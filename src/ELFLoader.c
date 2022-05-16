#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifndef WIN32
#include <sys/mman.h>
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include "debug.h"
#include "minimal_elf.h"
#include "ELFLoader.h"
#include "beacon_compatibility.h"

#ifdef LIBRARY
__attribute__ ((visibility ("default")))
#endif
unsigned char* unhexlify(unsigned char* value, int *outlen){
    unsigned char* retval = NULL;
    char byteval[3] = {0};
    int counter = 0;
    int counter2 = 0;
    char character = 0;
    if (value == NULL){
        return NULL;
    }
    DEBUG_PRINT("Unhexlify Strlen: %lu\n", (long unsigned int)strlen((char*)value));
    if (value == NULL || strlen((char*)value)%2 != 0){
        DEBUG_PRINT("Either value is NULL, or the hexlified string isn't valid\n");
        goto errcase;
    }

    retval = calloc(strlen((char*)value)+1, 1);
    if (retval == NULL){
        goto errcase;
    }

    counter2 = 0;
    for (counter = 0; counter < strlen((char*)value); counter += 2){
        memcpy(byteval, value+counter, 2);
        character = strtol(byteval, NULL, 16);
        memcpy(retval+counter2, &character, 1);
        counter2++;
    }
    *outlen = counter2;

errcase:
    return retval;
}

#ifdef LIBRARY
__attribute__ ((visibility ("default")))
#endif
int ELFRunner(char* functionName, unsigned char* elfObjectData, unsigned int size, unsigned char* argumentdata, int argumentSize){
#if defined(__amd64__) || defined(__x86_64__) || defined(__i386__) || (defined(DEBUG) && defined(ELFRUNNERTEST))
    ELFInfo_t elfinfo;
    int counter = 0;
    int c2 = 0;
    int (*ptr)(unsigned char*, int) = (int (*)(unsigned char *, int))NULL;
    int retcode = 0;
    int tempOffsetCounter = 0;
   
    memset(&elfinfo, 0, sizeof(ELFInfo_t));
    
    elfinfo.Header = (Elf_Ehdr*) elfObjectData;
    /* Verify that the data is an ELF file. */
    if (elfObjectData[0] != '\x7f' || elfObjectData[1] != 'E' || elfObjectData[2] != 'L' || elfObjectData[3] != 'F'){
        DEBUG_PRINT("Not an elf file\n");
        retcode = 1;
        goto errorcase;
    }
    /* Verify that the ELF file is a relocatable file, and not a shared object or binary */
    if (elfinfo.Header->e_type != 1){
        DEBUG_PRINT("ELF Type isn't relocatable type, bailing...\n");
        retcode = 2;
        goto errorcase;
    }
    if (elfinfo.Header->e_machine != COMPILEDMACHINEARCH){
        DEBUG_PRINT("ERROR, not the machine type your running on\n");
        retcode = -1;
        goto errorcase;
    }
    DEBUG_PRINT("ELF Object Data: %p\n", elfObjectData);
    DEBUG_PRINT("ELF Type: %d\n", elfinfo.Header->e_type);
    DEBUG_PRINT("ELF Machine: %d\n", elfinfo.Header->e_machine);
    DEBUG_PRINT("ELF Version: %d\n", elfinfo.Header->e_version);
    DEBUG_PRINT("ELF Entry: 0x%lx\n", elfinfo.Header->e_entry);
    DEBUG_PRINT("ELF ProgramHeaderOffset: 0x%lx\n", elfinfo.Header->e_phoff);
    DEBUG_PRINT("ELF SectionHeaderOffset: 0x%lx\n", elfinfo.Header->e_shoff);
    DEBUG_PRINT("ELF Flags: 0x%x\n", elfinfo.Header->e_flags);
    DEBUG_PRINT("ELF Header Size: %d\n", elfinfo.Header->e_ehsize);
    DEBUG_PRINT("ELF Program Header Entry Size: %d\n", elfinfo.Header->e_phentsize);
    DEBUG_PRINT("ELF Program Header Entry Count: %d\n", elfinfo.Header->e_phnum);
    DEBUG_PRINT("ELF Section Header Entry Size: %d\n", elfinfo.Header->e_shentsize);
    DEBUG_PRINT("ELF Section Header Entry Count: %d\n", elfinfo.Header->e_shnum);
    DEBUG_PRINT("ELF Section Header Table Index Entry: %d\n", elfinfo.Header->e_shstrndx);
    
    /* Set all the headers and sizes */
    elfinfo.progHeader = (Elf_Phdr*)(elfObjectData + elfinfo.Header->e_phoff);
    elfinfo.sectHeader = (Elf_Shdr*)(elfObjectData + elfinfo.Header->e_shoff);
    elfinfo.progHeaderSize = elfinfo.Header->e_phnum;
    elfinfo.sectHeaderSize = elfinfo.Header->e_shnum;
    
    /* This is usually whats important for standard in memory loaders, but since this is loading
     * object files, this isn't actually used. Leaving in the loader so I can extend later on.*/
    DEBUG_PRINT("Working with program headers, Count: %d\n", elfinfo.progHeaderSize);
    for (counter = 0; counter < elfinfo.progHeaderSize; counter++){
        DEBUG_PRINT("Program Header Entry Counter: %d\n", counter);
        DEBUG_PRINT("\tOffset: 0x%lx\n", elfinfo.progHeader[counter].p_offset);
    }
    /* End the program header loop */
    
    elfinfo.sectionMappings = calloc(elfinfo.sectHeaderSize*sizeof(char*), 1);
    elfinfo.sectionMappingProts = calloc(elfinfo.sectHeaderSize*sizeof(int), 1);
    /* Make sure that the section mappings and protections array is allocated, if not then error out. */
    if (elfinfo.sectionMappings == NULL || elfinfo.sectionMappingProts == NULL){
        DEBUG_PRINT("Failed to setup sectionMappings\n");
        retcode = 3;
        goto errorcase;
    }
    #ifdef WIN32
    elfinfo.tempOffsetTable = VirtualAlloc(NULL, 255*ThunkTrampolineSize, MEM_COMMIT|MEM_RESERVE|MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
    #else
    elfinfo.tempOffsetTable = mmap(NULL, 255*ThunkTrampolineSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    tempOffsetCounter += 0x5000;
    #endif
    elfinfo.tempOffsetCounter = 0;
    if (elfinfo.tempOffsetTable == NULL || elfinfo.tempOffsetTable == (void*)-1){
        DEBUG_PRINT("Failed to allocate the hacky GOT/Thunk function table.\n");
        retcode = 4;
        goto errorcase;
    }
    
    DEBUG_PRINT("Working over section headers, Count: %d\n", elfinfo.sectHeaderSize);
    for (counter = 0; counter < elfinfo.sectHeaderSize; counter++){
        int sectionProts = PROT_READ | PROT_WRITE;
        DEBUG_PRINT("Section Header Entry Counter: %d\n", counter);
        DEBUG_PRINT("\tName is %d\n", elfinfo.sectHeader[counter].sh_name);
        DEBUG_PRINT("\tType is 0x%x\n", elfinfo.sectHeader[counter].sh_type);
        DEBUG_PRINT("\tFlags are 0x%lx\n", elfinfo.sectHeader[counter].sh_flags);
        /* Identify the memory permissions here */
        if (elfinfo.sectHeader[counter].sh_flags & 0x1){
            DEBUG_PRINT("\t\tWriteable Section\n");
            sectionProts = PROT_READ | PROT_WRITE;
        }
        if (elfinfo.sectHeader[counter].sh_flags & 0x4){
            DEBUG_PRINT("\t\tExecutable Section\n");
            sectionProts = PROT_READ | PROT_EXEC;
        }
        if (elfinfo.sectHeader[counter].sh_size > 0 && elfinfo.sectHeader[counter].sh_type == SHT_PROGBITS){
            #ifdef WIN32
            elfinfo.sectionMappings[counter] = VirtualAlloc(NULL, elfinfo.sectHeader[counter].sh_size, MEM_COMMIT|MEM_RESERVE|MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
            #else
            elfinfo.sectionMappings[counter] = mmap(elfinfo.tempOffsetTable+tempOffsetCounter, elfinfo.sectHeader[counter].sh_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            tempOffsetCounter += 0x5000;
            #endif
            if (elfinfo.sectionMappings[counter] == NULL || elfinfo.sectionMappings[counter] == (void*)-1){
                DEBUG_PRINT("\t\t\tFailed to allocate memory for section\n");
                retcode = 5;
                goto errorcase;
            }
            memcpy(elfinfo.sectionMappings[counter], elfObjectData+elfinfo.sectHeader[counter].sh_offset, elfinfo.sectHeader[counter].sh_size);
        }
        else{
            /* Not allocating memory because the section isn't needed for the program to run, just used to link. */
            DEBUG_PRINT("\t\t\tNot allocating memory for section\n");
            elfinfo.sectionMappings[counter] = NULL;
        }
        elfinfo.sectionMappingProts[counter] = sectionProts;
        
        DEBUG_PRINT("\tAddr is 0x%lx\n", elfinfo.sectHeader[counter].sh_addr);
        DEBUG_PRINT("\tOffset is 0x%lx\n", elfinfo.sectHeader[counter].sh_offset);
        DEBUG_PRINT("\tSize is %ld\n", elfinfo.sectHeader[counter].sh_size);
        DEBUG_PRINT("\tLink is %d\n", elfinfo.sectHeader[counter].sh_link);
        DEBUG_PRINT("\tInfo is %d\n", elfinfo.sectHeader[counter].sh_info);
        DEBUG_PRINT("\tAddrAlign is %ld\n", elfinfo.sectHeader[counter].sh_addralign);
        DEBUG_PRINT("\tEntSize is %ld\n", elfinfo.sectHeader[counter].sh_entsize);
        /* Locate the sections that we want to keep track to */
        switch (elfinfo.sectHeader[counter].sh_type){
            case SHT_SYMTAB:
                DEBUG_PRINT("\t\tSymbol Table\n");
                elfinfo.symbolTable = (Elf_Sym*)(elfObjectData + elfinfo.sectHeader[counter].sh_offset);
                elfinfo.stringTable = (char*)(elfObjectData + elfinfo.sectHeader[elfinfo.sectHeader[counter].sh_link].sh_offset);
                DEBUG_PRINT("\t\tSymbolTable: %p\n", elfinfo.symbolTable);
                DEBUG_PRINT("\t\tStringTable: %p\n", elfinfo.stringTable);
                break;
            case SHT_STRTAB:
                DEBUG_PRINT("\t\tString Table\n");
                elfinfo.sectionStringTable = (char*)(elfObjectData + elfinfo.sectHeader[counter].sh_offset);
                break;
            default:
                DEBUG_PRINT("\t\tCase Not Handled\n");
                break;
        }
    }

    DEBUG_PRINT("\nWorking over section headers, Round 2, Count: %d\n", elfinfo.sectHeaderSize);
    for (counter = 0; counter < elfinfo.sectHeaderSize; counter++){
        DEBUG_PRINT("Section Header Entry Counter: %d\n", counter);
        #ifdef DEBUG
        char* sym = elfinfo.sectionStringTable + elfinfo.sectHeader[counter].sh_name;
        DEBUG_PRINT("\tName is %s\n", sym);
        #endif
        Elf_Rel* rel = (Elf_Rel*)(elfObjectData + elfinfo.sectHeader[counter].sh_offset);
        DEBUG_PRINT("\tType is 0x%x\n", elfinfo.sectHeader[counter].sh_type);
        DEBUG_PRINT("\tFlags are 0x%lx\n", elfinfo.sectHeader[counter].sh_flags);
        DEBUG_PRINT("\tAddr is 0x%lx\n", elfinfo.sectHeader[counter].sh_addr);
        DEBUG_PRINT("\tOffset is 0x%lx\n", elfinfo.sectHeader[counter].sh_offset);
        DEBUG_PRINT("\tSize is %ld\n", elfinfo.sectHeader[counter].sh_size);
        DEBUG_PRINT("\tLink is %d\n", elfinfo.sectHeader[counter].sh_link);
        DEBUG_PRINT("\tInfo is %d\n", elfinfo.sectHeader[counter].sh_info);
        DEBUG_PRINT("\tAddrAlign is %ld\n", elfinfo.sectHeader[counter].sh_addralign);
        DEBUG_PRINT("\tEntSize is %ld\n", elfinfo.sectHeader[counter].sh_entsize);
        /* Handle the relocations here */
        if (elfinfo.sectHeader[counter].sh_type == SHT_REL_TYPE){
            DEBUG_PRINT("\tRelocation Entries:\n");
            for (c2 = 0; c2 < elfinfo.sectHeader[counter].sh_size / sizeof(Elf_Rel); c2++){
                char* relocStr = elfinfo.stringTable + elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_name;
                char WorkingTrampoline[ThunkTrampolineSize];
                memcpy(WorkingTrampoline, ThunkTrampoline, ThunkTrampolineSize);
                DEBUG_PRINT("\t\tSymbol: %s\n", relocStr);
                DEBUG_PRINT("\t\tType: 0x%lx\n", ELF_R_TYPE(rel[c2].r_info));
                DEBUG_PRINT("\t\tSymbolValue: 0x%lx\n", elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_value);
                /* This is the section number where the relocation lives in at x + offset, if its 0 then its a symbol to get
                 * so get the address, store the address, increase the symbol count, and then continue */
                DEBUG_PRINT("\t\tShndx: 0x%x\n", elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx);
                if (elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx == 0){
                    /* This function is a function not defined in the object file, so if we can't resolve it then bail. */
                    void* symaddress = NULL;
                    int32_t relativeOffsetFunc = 0;
                    symaddress = internalFunctionLookup(relocStr);
                    /* TODO: Add an in process function lookup here too */
                    if (symaddress == NULL){
                        DEBUG_PRINT("Failed to find a function!!!!!\n");
                        retcode = 6;
                        goto errorcase;
                    }
                    DEBUG_PRINT("\t\tFound Function Address: %p\n", symaddress);
                    /* Copy over the symaddress to the location of the trampoline */
                    memcpy(WorkingTrampoline+THUNKOFFSET, &symaddress, sizeof(void*));
                    /* Copy the trampoline bytes over to the tempOffsetTable so relocations work */
                    DEBUG_PRINT("TempOffsetCounter: %d\n", elfinfo.tempOffsetCounter);
                    memcpy(elfinfo.tempOffsetTable+(elfinfo.tempOffsetCounter*ThunkTrampolineSize), WorkingTrampoline, ThunkTrampolineSize);
                    /* Calculate the relative offset of the function trampoline */
                    /* The logic to handle x86_64 is different then x86, so ifdef'ing these out for now */
                    #if defined(__amd64__) || defined(__x86_64__)
                    relativeOffsetFunc = (elfinfo.tempOffsetTable + (elfinfo.tempOffsetCounter *ThunkTrampolineSize))-(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset)+rel[c2].r_addend;
                    DEBUG_PRINT("\t\tRelativeOffsetFunc: 0x%x\n", relativeOffsetFunc);
                    /* Copy over the relative offset to the trampoline table */
                    memcpy(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset, &relativeOffsetFunc, 4);
                    #elif defined(__i386__)
                    /* Need to correct this for x86 and 32 bit arm targets, think its good now. */
                    memcpy(&relativeOffsetFunc, elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset, 4);
                    relativeOffsetFunc += (elfinfo.tempOffsetTable + (elfinfo.tempOffsetCounter *ThunkTrampolineSize))-(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset);
                    memcpy(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset, &relativeOffsetFunc, 4);
                    #else
                    DEBUG_PRINT("ERROR: Not configured for this architecture\n");
                    #endif
                    /* Once set increment the Thunk Trampoline counter to the next one */
                    elfinfo.tempOffsetCounter+=1;
                    
                }
                else if (elfinfo.sectHeader[counter].sh_flags== 0x40){
                    /* Handle the relocations for values and functions included in the object file */
                    /* NOTE: If sh_flags == 0x40, then sh_info contains the section the relocation applies too */
                    #if defined(__amd64__) || defined(__x86_64__)
                    int32_t relativeOffset = (elfinfo.sectionMappings[elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx])-(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset)+rel[c2].r_addend + elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_value;
                    #elif defined(__i386__)
                    int32_t relativeOffset = 0;
                    if (ELF_R_TYPE(rel[c2].r_info) == R_386_32){
                        DEBUG_PRINT("\t\t32bit Direct\n");
                        memcpy(&relativeOffset, elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset, 4);
                        //relativeOffset = (elfinfo.sectionMappings[elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx])-(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset)+relativeOffset + elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_value;
                        relativeOffset += elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_value;
                        relativeOffset += (int32_t)(elfinfo.sectionMappings[elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx]);
                    }
                    else if (ELF_R_TYPE(rel[c2].r_info) == R_386_PC32){
                        DEBUG_PRINT("\t\tPC relative Address\n");
                        memcpy(&relativeOffset, elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset, 4);
                        relativeOffset = (elfinfo.sectionMappings[elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx])-(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset)+relativeOffset + elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_value;
                    }
                    #else
                    int32_t relativeOffset = 0;
                    #endif
                    DEBUG_PRINT("\t\tFirstAddress(NoAddend): %p\n", (elfinfo.sectionMappings[elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx]));
                    
                    #if defined(__amd64__) || defined(__x86_64__)
                    DEBUG_PRINT("\t\tFirstAddress: %p\n", (elfinfo.sectionMappings[elfinfo.symbolTable[ELF_R_SYM(rel[c2].r_info)].st_shndx]+rel[c2].r_addend));
                    #endif
                    DEBUG_PRINT("\t\tSecondAddress(NoOffset): %p\n", (elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]));
                    DEBUG_PRINT("\t\tSecondAddress: %p\n", (elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset));
                    DEBUG_PRINT("\t\tRelativeOffset: 0x%x\n", relativeOffset);
                    /* Copy over the relative offset of the value to the section+offset */
                    memcpy(elfinfo.sectionMappings[elfinfo.sectHeader[counter].sh_info]+rel[c2].r_offset, &relativeOffset, 4);
                }
                DEBUG_PRINT("\t\tInfo: 0x%lx\n", rel[c2].r_info);
                DEBUG_PRINT("\t\tOffset: 0x%lx\n", rel[c2].r_offset);
                #if defined(__amd64__) || defined(__x86_64__)
                DEBUG_PRINT("\t\tAddend: 0x%lx\n", rel[c2].r_addend);
                #endif
                DEBUG_PRINT("\t\t----------------------------------------------------------\n");
            }
        }
        
        /* Handle the symbols here, get the entry points and all that */
        if (elfinfo.sectHeader[counter].sh_type == SHT_SYMTAB){
            for (c2 = 0; c2 < elfinfo.sectHeader[counter].sh_size / sizeof(Elf_Sym); c2 += 1){
                Elf_Sym* syms = (Elf_Sym*)(elfObjectData + elfinfo.sectHeader[counter].sh_offset);
                //DEBUG_PRINT("\t\t0x%x\n", syms[c2].st_name);
                DEBUG_PRINT("\t\tSymbolName: %s\n", elfinfo.stringTable + syms[c2].st_name);
                if (strcmp(functionName, elfinfo.stringTable + syms[c2].st_name) == 0){
                    DEBUG_PRINT("\t\t\tFOUND GO!\n");
                    ptr = (int (*)(unsigned char *, int))elfinfo.sectionMappings[syms[c2].st_shndx] + syms[c2].st_value;
                }
                DEBUG_PRINT("\t\tSymbolSectionIndex: %d\n", syms[c2].st_shndx);
                if (elfinfo.sectionMappings != NULL && syms[c2].st_shndx < elfinfo.sectHeaderSize && syms[c2].st_shndx != 0){
                    DEBUG_PRINT("\t\tSymbolAddress(real): %p\n", elfinfo.sectionMappings[syms[c2].st_shndx] + syms[c2].st_value);
                }
            }
        }
    }
    
    DEBUG_PRINT("TempOffsetTable: %p\n", elfinfo.tempOffsetTable);
    #ifdef WIN32
    DEBUG_PRINT("Skipping mprotect code for windows, already executable.\n");
    #else
    if (mprotect(elfinfo.tempOffsetTable, 255*ThunkTrampolineSize, PROT_READ | PROT_EXEC) != 0){
        DEBUG_PRINT("Failed to mprotect the thunk table\n");
    }
    #endif
    /* TODO: Set all the permissions of the sectionMappings, sectionMappingProts, and tempOffsetTable */
    for (counter = 0; counter < elfinfo.sectHeaderSize; counter++){
        DEBUG_PRINT("Section #%d mapped at %p\n", counter, elfinfo.sectionMappings[counter]);
        if (elfinfo.sectionMappings[counter] != NULL){
            #ifdef WIN32
            DEBUG_PRINT("Not doing mprotect for windows, already executable.\n");
            #else
            if (mprotect(elfinfo.sectionMappings[counter], elfinfo.sectHeader[counter].sh_size, elfinfo.sectionMappingProts[counter]) != 0){
                DEBUG_PRINT("Failed to protect memory\n");
            }
            #endif
        }
    }
    DEBUG_PRINT("Trying to run ptr......\n");
    /* NOTE: Change this to pass in arguments for ones that use it */
    (void)ptr(argumentdata, argumentSize);
    DEBUG_PRINT("Returned from ptr\n");
    
cleanup:
    DEBUG_PRINT("Cleaning up...\n");
    for (counter = 0; counter < elfinfo.sectHeaderSize; counter++){
        DEBUG_PRINT("Freeing Section #%d\n", counter);
        if (elfinfo.sectionMappings != NULL){
            if (elfinfo.sectionMappings[counter] != NULL){
                #ifdef WIN32
                VirtualFree(elfinfo.sectionMappings[counter], 0, MEM_RELEASE);
                #else
                if (munmap(elfinfo.sectionMappings[counter], elfinfo.sectHeader[counter].sh_size)  != 0){
                    DEBUG_PRINT("Failed to unmap memory\n");
                }
                #endif
            }
        }
    }
    if (elfinfo.tempOffsetTable){
        #ifdef WIN32
        VirtualFree(elfinfo.tempOffsetTable, 0, MEM_RELEASE);
        #else
        munmap(elfinfo.tempOffsetTable, 255*ThunkTrampolineSize);
        #endif
    }
    if (elfinfo.sectionMappings){
        free(elfinfo.sectionMappings);
    }
    if (elfinfo.sectionMappingProts){
        free(elfinfo.sectionMappingProts);
    }
 
    goto retlab;
   
retlab:
    DEBUG_PRINT("Returning\n");
    return retcode;
    
errorcase:
    DEBUG_PRINT("ERRORCASE!!!!\n");
    goto cleanup;
#else
    return -1;
#endif      /* Wrapper ifdef for supported arch's */
}

#ifdef TESTING_MAIN
int main(int argc, char** argv, char** envp)
{
    //int (*ptr)(char*, int*);
    unsigned char* buf = NULL;
    int size = 0;
    FILE* elf = NULL;
    int checkcode = 0;
    char* outputdata = NULL;
    char* argumentdata = NULL;
    int argumentdatalen = 0;
    int outputdataLen = 0;

    if (argc < 2){
        printf("%s ./path/to/objectfile.o\n", argv[0]);
        return 0;
    }
    elf = fopen(argv[1], "rb");
    if (elf == NULL){
        printf("ERROR: File doesn't exist\n");
        return 0;
    }
    fseek(elf, 0, SEEK_END);
    size = ftell(elf);
    fseek(elf, 0, SEEK_SET);
    buf = calloc(size, 1);
    if (buf == NULL){
        return 0;
    }
    (void)fread(buf, 1, size, elf);
    argumentdata = (char*)unhexlify((unsigned char*)argv[2], &argumentdatalen);
    checkcode = ELFRunner("go", buf, size, (unsigned char*)argumentdata, argumentdatalen );
    if (checkcode == 0){
        outputdata = BeaconGetOutputData(&outputdataLen);
        printf("Output data : %s\n", outputdata);
        free(outputdata);
    }


    free(buf);
    fclose(elf);
    return 0;
}
#endif
