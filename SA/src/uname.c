#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/utsname.h>

#include "beacon_api.h"

int run_uname(void){
    struct utsname values;
    int retcode = 0;
    char* osversion = getOSName();
    if (strcmp("lin", osversion)==0){
        retcode = uname(&values);
        BeaconPrintf(CALLBACK_OUTPUT, "%s %s %s %s %s\n", values.sysname, values.nodename, values.release, values.version, values.machine);
    }
    else{
        BeaconPrintf(CALLBACK_OUTPUT, "Not supported\n");
    }
    return 0;
}

int go(char* indata, int inlen){
    
    (void)run_uname();
    return 0;
}

#ifdef TEST_UNAME
int main(void){
    return run_uname();
}
#endif
