

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ACTEncode(char *pcmIn,unsigned int pcmLen, char* bitOut, unsigned int *bitOutLen)
{
	memmove(bitOut, pcmIn, pcmLen);
	*bitOutLen = pcmLen;
}
