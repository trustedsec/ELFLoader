#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "beacon_api.h"

int run_pwd(void){
    int retcode = 0;
    char workingdir[256] = {0};
    char* workdir = NULL;
    workdir = getcwd(workingdir, 255);
    if (workdir == NULL){
        BeaconPrintf(CALLBACK_OUTPUT, "ERROR\n");
    }
    else{
        BeaconPrintf(CALLBACK_OUTPUT, "%s\n", workingdir);
    }
    return 0;
}

int go(char* indata, int inlen){
    
    (void)run_pwd();
    return 0;
}

#ifdef TEST_PWD
int main(void){
    return run_pwd();
}
#endif
