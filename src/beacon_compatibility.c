/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 * 
 * Built off of the beacon.h file provided to build for CS.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#ifndef WIN32
#include <sys/mman.h>
#include <dlfcn.h>
#else
#include <windows.h>
#endif


#include "beacon_compatibility.h"

#define X86PATH "System32"
#define X64PATH "sysnative"
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#define INTERNAL_DEFAULT_LIBRARY ((void*) -2)
#else
#define INTERNAL_DEFAULT_LIBRARY NULL
#endif

extern char** environ;

/* Data Parsing */

beacon_function_t BeaconInternalMapping[18] = {
    {"BeaconDataParse", (void*)BeaconDataParse},
    {"BeaconDataInt", (void*)BeaconDataInt},
    {"BeaconDataShort", (void*)BeaconDataShort},
    {"BeaconDataLength", (void*)BeaconDataLength},
    {"BeaconDataExtract", (void*)BeaconDataExtract},
    {"BeaconFormatAlloc", (void*)BeaconFormatAlloc},
    {"BeaconFormatReset", (void*)BeaconFormatReset},
    {"BeaconFormatFree", (void*)BeaconFormatFree},
    {"BeaconFormatAppend", (void*)BeaconFormatAppend},
    {"BeaconFormatPrintf", (void*)BeaconFormatPrintf},
    {"BeaconFormatToString", (void*)BeaconFormatToString},
    {"BeaconFormatInt", (void*)BeaconFormatInt},
    {"BeaconPrintf", (void*)BeaconPrintf},
    {"BeaconOutput", (void*)BeaconOutput},
    {"BeaconIsAdmin", (void*)BeaconIsAdmin},
    {"getEnviron", (void*)getEnviron},
    {"getOSName", (void*)getOSName}
};


char** getEnviron(void){
    return environ;
}

char* getOSName(void){
#ifdef __APPLE__
    return "apple";
#elif __FreeBSD__
    return "freebsd";
#elif __OpenBSD__
    return "openbsd";
#elif __linux__
    return "lin";
#else
    return "unk";
#endif
}

#ifdef _WIN32
/* A hacky compatible dlsym function for use on windows systems. 
 * TODO: Implement a internal function lookup function for all OS's that
 *       will lookup internal functions, and global functions.*/
void *dlsym(void *handle, const char *symbol){
    static HMODULE DefaultHandle = NULL;
    void* PointerValue = NULL;
    if (handle == INTERNAL_DEFAULT_LIBRARY && DefaultHandle == NULL){
        DefaultHandle = LoadLibraryA("msvcrt.dll");
    }
    if (DefaultHandle != NULL){
        PointerValue = GetProcAddress(DefaultHandle, symbol);
    }
    return PointerValue;
}
#endif


void* internalFunctionLookup(char* symbolName){
    void* functionaddress = NULL;
    int tempcounter = 0;
    for (tempcounter = 0; tempcounter < BEACONINTERNALMAPPINGCOUNT; tempcounter++){
        if (strcmp(symbolName, BeaconInternalMapping[tempcounter].functionName) == 0){
            #ifdef DEBUG
            printf("\t\tInternalFunction: %s\n", symbolName);
            #endif
            functionaddress = BeaconInternalMapping[tempcounter].function;
            return functionaddress;
        }
    }
    /* If not an internal function, then its an external one */
    if (functionaddress == NULL){
        functionaddress = dlsym(INTERNAL_DEFAULT_LIBRARY, symbolName);
    }
    return functionaddress;
}


uint32_t swap_endianess(uint32_t indata){
    uint32_t testint = 0xaabbccdd;
    uint32_t outint = indata;
    if (((unsigned char*)&testint)[0] == 0xdd){
        ((unsigned char*)&outint)[0] = ((unsigned char*)&indata)[3];
        ((unsigned char*)&outint)[1] = ((unsigned char*)&indata)[2];
        ((unsigned char*)&outint)[2] = ((unsigned char*)&indata)[1];
        ((unsigned char*)&outint)[3] = ((unsigned char*)&indata)[0];
    }
    return outint;
}

char* beacon_compatibility_output = NULL;
int beacon_compatibility_size = 0;
int beacon_compatibility_offset = 0;

void BeaconDataParse(datap* parser, char* buffer, int size){
    if (parser == NULL){
        return;
    }
    parser->original = buffer;
    parser->buffer = buffer;
    parser->length = size-4;
    parser->size = size-4;
    parser->buffer += 4;
    return;
}

int BeaconDataInt(datap* parser){
    int32_t fourbyteint = 0;
    if (parser->length < 4){
        return 0;
    }
    memcpy(&fourbyteint, parser->buffer, 4);
    parser->buffer += 4;
    parser->length -= 4;
    return (int)fourbyteint;
}

short BeaconDataShort(datap* parser){
    int16_t retvalue = 0;
    if (parser->length < 2){
        return 0;
    }
    memcpy(&retvalue, parser->buffer, 2);
    parser->buffer += 2;
    parser->length -= 2;
    return (short)retvalue;
}

int BeaconDataLength(datap* parser){
    return parser->length;
}

char* BeaconDataExtract(datap* parser, int* size){
    uint32_t length = 0;
    char* outdata = NULL;
    /*Length prefixed binary blob, going to assume uint32_t for this.*/
    if (parser->length < 4){
        return NULL;
    }
    memcpy(&length, parser->buffer, 4);
    parser->buffer += 4;
    
    outdata = parser->buffer;
    if (outdata == NULL){
        return NULL;
    }
    parser->length -=4;
    parser->length -= length;
    parser->buffer += length;
    if (size != NULL && outdata != NULL){
        *size = length;
    }
    return outdata;
}

/* format API */

void BeaconFormatAlloc(formatp* format, int maxsz){
    if (format == NULL){
        return;
    }
    format->original = calloc(maxsz, 1);
    format->buffer = format->original;
    format->length = 0;
    format->size = maxsz;
    return;
}

void BeaconFormatReset(formatp* format){
    memset(format->original, 0, format->size);
    format->buffer = format->original;
    format->length = format->size;
    return;
}

void BeaconFormatFree(formatp* format){
    if (format == NULL){
        return;
    }
    if (format->original){
        free(format->original);
        format->original = NULL;
    }
    format->buffer = NULL;
    format->length = 0;
    format->size = 0;
    return;
}

void BeaconFormatAppend(formatp* format, char* text, int len){
    memcpy(format->buffer, text, len);
    format->buffer+= len;
    format->length+= len;
    return;
}

void BeaconFormatPrintf(formatp* format, char* fmt, ...){
    /*Take format string, and sprintf it into here*/
    va_list args;
    int length = 0;

    va_start (args, fmt);
    length = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (format->length + length > format->size){
        return;
    }

    va_start (args, fmt);
    (void)vsnprintf(format->buffer, length, fmt, args);
    va_end(args);
    format->length += length;
    format->buffer+= length;
    return;
}


char* BeaconFormatToString(formatp* format, int* size){
    *size = format->length;
    return format->original;
}

void BeaconFormatInt(formatp* format, int value){
    uint32_t indata = value;
    uint32_t outdata = 0;
    if (format->length + 4 > format->size){
        return;
    }
    outdata = swap_endianess(indata);
    memcpy(format->buffer, &outdata, 4);
    format->length += 4;
    format->buffer += 4;
    return;
}
/* Main output functions */

void BeaconPrintf(int type, char* fmt, ...){
    /* Change to maintain internal buffer, and return after done running. */
    int length = 0;
    char* tempptr = NULL;
    va_list args;
    #ifdef DEBUG
    va_start (args, fmt);
    vprintf(fmt, args);
    va_end(args);
    #endif

    va_start (args, fmt);
    length = vsnprintf(NULL, 0, fmt, args)+1;
    va_end(args);
    tempptr = realloc(beacon_compatibility_output, beacon_compatibility_size+length+1);
    if (tempptr == NULL){
        return;
    }
    beacon_compatibility_output = tempptr;
    memset(beacon_compatibility_output+beacon_compatibility_offset, 0, length+1);
    va_start (args, fmt);
    length = vsnprintf(beacon_compatibility_output+beacon_compatibility_offset, length, fmt, args);
    beacon_compatibility_size+=length;
    beacon_compatibility_offset+=length;
    va_end(args);
    return;
}

void BeaconOutput(int type, char* data, int len){
    char* tempptr = NULL;
    tempptr = realloc(beacon_compatibility_output, beacon_compatibility_size+len+1);
    beacon_compatibility_output = tempptr;
    if (tempptr == NULL){
        return;
    }
    memset(beacon_compatibility_output+beacon_compatibility_offset, 0, len+1);
    memcpy(beacon_compatibility_output+beacon_compatibility_offset, data, len);
    beacon_compatibility_size+=len;
    beacon_compatibility_offset+=len;
    return;
}

int BeaconIsAdmin(void){
    /* Leaving this to be implemented by people needing it */
    #ifdef DEBUG
    printf("BeaconIsAdmin Called\n");
    #endif
    return 0;
}

#ifdef LIBRARY
__attribute__ ((visibility ("default")))
#endif
char* BeaconGetOutputData(int *outsize){
    char* outdata = beacon_compatibility_output;
    *outsize = beacon_compatibility_size;
    beacon_compatibility_output = NULL;
    beacon_compatibility_size = 0;
    beacon_compatibility_offset = 0;
    return outdata;
}

