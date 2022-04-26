#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "beacon_api.h"

int test1(void){
    printf("This is a test2\n");
    return 0;
}

int go(char* indata, int* outlen){
    BeaconPrintf(CALLBACK_OUTPUT, "This is a test BeaconPrintf output\n");
    printf("This is a test\n");
    
    return 0;
}
