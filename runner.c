#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ELFRunner_include.h>

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

