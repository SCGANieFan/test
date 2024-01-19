
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void ACTDecode(char* bitIn, unsigned int bitInLen, char* pcmOut, unsigned int *pcmLen)
{
	memmove(pcmOut, bitIn, bitInLen);
	*pcmLen = bitInLen;
}