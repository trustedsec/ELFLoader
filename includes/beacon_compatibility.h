/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 * 
 * Built off of the beacon.h file provided to build for CS.
 */
#ifndef BEACON_COMPATIBILITY_H_
#define BEACON_COMPATIBILITY_H_
typedef struct beacon_function{
    char* functionName;
    void* function;
} beacon_function_t;

extern beacon_function_t BeaconInternalMapping[18];
#define BEACONINTERNALMAPPINGCOUNT 17

void* internalFunctionLookup(char* symbolName);
/* Structures as is in beacon.h */

typedef struct {
    char * original; /* the original buffer [so we can free it] */
    char * buffer;   /* current pointer into our buffer */
    int    length;   /* remaining length of data */
    int    size;     /* total size of this buffer */
} datap;

typedef struct {
    char * original; /* the original buffer [so we can free it] */
    char * buffer;   /* current pointer into our buffer */
    int    length;   /* remaining length of data */
    int    size;     /* total size of this buffer */
} formatp;

void    BeaconDataParse(datap * parser, char * buffer, int size);
int     BeaconDataInt(datap * parser);
short   BeaconDataShort(datap * parser);
int     BeaconDataLength(datap * parser);
char *  BeaconDataExtract(datap * parser, int * size);

void    BeaconFormatAlloc(formatp * format, int maxsz);
void    BeaconFormatReset(formatp * format);
void    BeaconFormatFree(formatp * format);
void    BeaconFormatAppend(formatp * format, char * text, int len);
void    BeaconFormatPrintf(formatp * format, char * fmt, ...);
char *  BeaconFormatToString(formatp * format, int * size);
void    BeaconFormatInt(formatp * format, int value);

#define CALLBACK_OUTPUT      0x0
#define CALLBACK_OUTPUT_OEM  0x1e
#define CALLBACK_ERROR       0x0d
#define CALLBACK_OUTPUT_UTF8 0x20


void   BeaconPrintf(int type, char * fmt, ...);
void   BeaconOutput(int type, char * data, int len);

/* Token Functions */
int BeaconIsAdmin();

uint32_t swap_endianess(uint32_t indata);

char* BeaconGetOutputData(int *outsize);
char** getEnviron(void);
char* getOSName(void);
#endif
