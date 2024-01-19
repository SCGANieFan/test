#ifndef __AudioCodecTest__H_
#define __AudioCodecTest__H_

void ACTEncode(char* pcmIn, unsigned int pcmLen, char* bitOut, unsigned int* bitOutLen);
void ACTDecode(char* bitIn, unsigned int bitInLen, char* pcmOut, unsigned int* pcmLen);

#endif