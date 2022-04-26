#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>


int go(char* indata, int* outlen){
    printf("This is a test\n");
    printf("UID is: %d\n", getuid());
    printf("This is another test\n");
    if (getuid() > 500){
        printf("Yep its greater than 500\n");
    }
    else{
        printf("Nope it isn't\n");
    }
    return 0;
}
