#ifndef ELFLOADER_INCLUDE_H_
#define ELFLOADER_INCLUDE_H_

int ELFRunner(char* functionName, unsigned char* elfObjectData, unsigned int size, unsigned char* argumentdata, int argumentSize);
char* BeaconGetOutputData(int *outsize);
unsigned char* unhexlify(unsigned char* value, int *outlen);

#endif
