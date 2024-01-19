/***********************************************************************
Copyright (c) 2006-2012, Skype Limited. All rights reserved. 
Redistribution and use in source and binary forms, with or without 
modification, (subject to the limitations in the disclaimer below) 
are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution.
- Neither the name of Skype Limited, nor the names of specific 
contributors, may be used to endorse or promote products derived from 
this software without specific prior written permission.
NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED 
BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#ifndef SKP_SILK_SDK_API_H
#define SKP_SILK_SDK_API_H

#include "SKP_Silk_control.h"
#include "SKP_Silk_typedef.h"
#include "SKP_Silk_errors.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SILK_MAX_FRAMES_PER_PACKET  5

/* Struct for TOC (Table of Contents) */
typedef struct {
    /* Number of 20 ms frames in packet     */
    SKP_int     framesInPacket;                             
    /* Sampling frequency in packet         */
    SKP_int     fs_kHz;                                     
    /* Does packet contain LBRR information */
    SKP_int     inbandLBRR;                                 
    /* Packet is corrupt                    */
    SKP_int     corrupt;                                    
    /* VAD flag for each frame in packet    */
    SKP_int     vadFlags[     SILK_MAX_FRAMES_PER_PACKET ]; 
    /* Signal type for each frame in packet */
    SKP_int     sigtypeFlags[ SILK_MAX_FRAMES_PER_PACKET ]; 
} SKP_Silk_TOC_struct;

/****************************************/
/* Encoder functions                    */
/****************************************/

/***********************************************/
/* Get size in bytes of the Silk encoder state */
/***********************************************/
SKP_int SKP_Silk_SDK_Get_Encoder_Size( 
    /* O:   Number of bytes in SILK encoder state           */
    SKP_int32                           *encSizeBytes   
);

/*************************/
/* Init or reset encoder */
/*************************/
SKP_int SKP_Silk_SDK_InitEncoder(
    /* I/O: State                                           */
    void                                *encState,      
    /* O:   Encoder Status                                  */
    SKP_SILK_SDK_EncControlStruct       *encStatus      
);

/***************************************/
/* Read control structure from encoder */
/***************************************/
SKP_int SKP_Silk_SDK_QueryEncoder(
    /* I:   State                                           */
    const void                          *encState,      
    /* O:   Encoder Status                                  */
    SKP_SILK_SDK_EncControlStruct       *encStatus      
);

/**************************/
/* Encode frame with Silk */
/**************************/
SKP_int SKP_Silk_SDK_Encode( 
    /* I/O: State                                           */
    void                                *encState,      
    /* I:   Control status                                  */
    const SKP_SILK_SDK_EncControlStruct *encControl,    
    /* I:   Speech sample input vector                      */
    const SKP_int16                     *samplesIn,     
    /* I:   Number of samples in input vector               */
    SKP_int                             nSamplesIn,     
    /* O:   Encoded output vector                           */
    SKP_uint8                           *outData,       
    /* I/O: Number of bytes in outData (input: Max bytes)   */
    SKP_int16                           *nBytesOut      
);

/****************************************/
/* Decoder functions                    */
/****************************************/

/***********************************************/
/* Get size in bytes of the Silk decoder state */
/***********************************************/
SKP_int SKP_Silk_SDK_Get_Decoder_Size( 
    /* O:   Number of bytes in SILK decoder state           */
    SKP_int32                           *decSizeBytes   
);

/*************************/
/* Init or Reset decoder */
/*************************/
SKP_int SKP_Silk_SDK_InitDecoder( 
    /* I/O: State                                           */
    void                                *decState       
);

/******************/
/* Decode a frame */
/******************/
SKP_int SKP_Silk_SDK_Decode(
    /* I/O: State                                           */
    void*                               decState,       
    /* I/O: Control Structure                               */
    SKP_SILK_SDK_DecControlStruct*      decControl,     
    /* I:   0: no loss, 1 loss                              */
    SKP_int                             lostFlag,       
    /* I:   Encoded input vector                            */
    const SKP_uint8                     *inData,       
    /* I:   Number of input bytes                           */
    const SKP_int                       nBytesIn,       
    /* O:   Decoded output speech vector                    */
    SKP_int16                           *samplesOut,    
    /* I/O: Number of samples (vector/decoded)              */
    SKP_int16                           *nSamplesOut    
);

/***************************************************************/
/* Find Low Bit Rate Redundancy (LBRR) information in a packet */
/***************************************************************/
//Redundancy »ﬂ”‡
void SKP_Silk_SDK_search_for_LBRR(
    /* I:   Encoded input vector                            */
    const SKP_uint8                     *inData,        
    /* I:   Number of input Bytes                           */
    const SKP_int                       nBytesIn,       
    /* I:   Offset from lost packet                         */
    SKP_int                             lost_offset,    
    /* O:   LBRR payload                                    */
    //payload ‘ÿ∫…,∏∫∫…
    SKP_uint8                           *LBRRData,      
    /* O:   Number of LBRR Bytes                            */
    SKP_int16                           *nLBRRBytes     
);

/**************************************/
/* Get table of contents for a packet */
/**************************************/
void SKP_Silk_SDK_get_TOC(
    /* I:   Encoded input vector                            */
    const SKP_uint8                     *inData,        
    /* I:   Number of input bytes                           */
    const SKP_int                       nBytesIn,       
    /* O:   Table of contents                               */
    SKP_Silk_TOC_struct                 *Silk_TOC       
);

/**************************/
/* Get the version number */
/**************************/
/* Return a pointer to string specifying the version */ 
const char *SKP_Silk_SDK_get_version();

#ifdef __cplusplus
}
#endif

#endif
