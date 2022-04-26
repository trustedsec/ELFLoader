#ifndef ELFLOADER_H_
#define ELFLOADER_H_

/*Defines to map Elf_Phdr to Elf32_Phdr and whatnot*/
#if UINTPTR_MAX == 0xffffffff
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Sym Elf32_Sym
#define Elf_Rel Elf32_Rel
#define Elf_Shdr    Elf32_Shdr
#define ELF_R_TYPE(i)   ((i) & 0xff)
#define ELF_R_SYM(x)    ((x) >> 8)
#define SHT_REL_TYPE    SHT_REL
#define X86
#else
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Sym Elf64_Sym
#define Elf_Rel Elf64_Rela
#define Elf_Shdr Elf64_Shdr
#define ELF_R_TYPE(i)   ((i) & 0xffffffff)
#define ELF_R_SYM(x)    ((x) >> 32)
#define SHT_REL_TYPE    SHT_RELA
#endif

typedef struct ELFInfo {
    Elf_Ehdr *Header;
    Elf_Phdr *progHeader;
    int progHeaderSize;
    Elf_Shdr *sectHeader;
    int sectHeaderSize;
    unsigned char* execPtr;
    Elf_Sym *symbolTable;
    char* stringTable;
    char* sectionStringTable;
    unsigned char** sectionMappings;
    int* sectionMappingProts;
    unsigned char* tempOffsetTable;
    int tempOffsetCounter;
} ELFInfo_t;
/* x86_64 */

#ifdef WIN32
#define PROT_READ PAGE_EXECUTE_READWRITE
#define PROT_WRITE PAGE_EXECUTE_READWRITE
#define PROT_EXEC PAGE_EXECUTE_READWRITE
#endif

#if defined(__amd64__) || defined(__x86_64__)
unsigned char* ThunkTrampoline = (unsigned char*)"\x48\xb8\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xEE\xff\xe0";
int ThunkTrampolineSize = 12;
#define THUNKOFFSET 2
#define COMPILEDMACHINEARCH EM_X86_64
#endif

/* x86 */
#if defined(__i386__)
unsigned char* ThunkTrampoline = (unsigned char*)"\x68\x00\x00\x00\x00\x58\xff\xe0";
int ThunkTrampolineSize = 8;
#define THUNKOFFSET 1
#define COMPILEDMACHINEARCH EM_386
#endif

int ELFRunner(char* functionName, unsigned char* elfObjectData, unsigned int size, unsigned char* argumentdata, int argumentSize);
#endif
