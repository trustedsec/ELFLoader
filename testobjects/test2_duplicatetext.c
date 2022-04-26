#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

int test1(void){
    printf("This is the test1 function\n");
    return 0;
}

int test2(void){
    printf("This is the test2 function\n");
    return 0;
}

int test3(void){
    printf("This is the test3 function\n");
    return 0;
}


int go(char* indata, int* outlen){
    (void)test1();
    (void)test2();
    (void)test3();
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
