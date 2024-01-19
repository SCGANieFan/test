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

#ifndef SKP_SILK_STRUCTS_H
#define SKP_SILK_STRUCTS_H

#include "SKP_Silk_typedef.h"
#include "SKP_Silk_SigProc_FIX.h"
#include "SKP_Silk_define.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************************/
/* Noise shaping quantization state */
/************************************/
typedef struct {
    /* Buffer for quantized output signal */
    SKP_int16   xq[           2 * MAX_FRAME_LENGTH ]; 
    SKP_int32   sLTP_shp_Q10[ 2 * MAX_FRAME_LENGTH ];
    SKP_int32   sLPC_Q14[ MAX_FRAME_LENGTH / NB_SUBFR + NSQ_LPC_BUF_LENGTH ];
    SKP_int32   sAR2_Q14[ MAX_SHAPE_LPC_ORDER ];
    SKP_int32   sLF_AR_shp_Q12;
    SKP_int     lagPrev;
    SKP_int     sLTP_buf_idx;
    SKP_int     sLTP_shp_buf_idx;
    SKP_int32   rand_seed;
    SKP_int32   prev_inv_gain_Q16;
    SKP_int     rewhite_flag;
} SKP_Silk_nsq_state; /* FIX*/

/* Struct for Low BitRate Redundant (LBRR) information */
typedef struct {
    SKP_uint8   payload[ MAX_ARITHM_BYTES ];    
    /* Number of bytes in payload                               */
    SKP_int     nBytes;                         
    /* Tells how the payload should be used as FEC              */
    SKP_int     usage;                          
} SKP_SILK_LBRR_struct;

/********************************/
/* VAD state                    */
/********************************/
typedef struct {
    /* Analysis filterbank state: 0-8 kHz                       */
    SKP_int32   AnaState[ 2 ];                  
    /* Analysis filterbank state: 0-4 kHz                       */
    SKP_int32   AnaState1[ 2 ];                 
    /* Analysis filterbank state: 0-2 kHz                       */
    SKP_int32   AnaState2[ 2 ];                 
    /* Subframe energies                                        */
    SKP_int32   XnrgSubfr[ VAD_N_BANDS ];       
    /* Smoothed energy level in each band                       */
    SKP_int32   NrgRatioSmth_Q8[ VAD_N_BANDS ]; 
    /* State of differentiator in the lowest band               */
    SKP_int16   HPstate;                        
    /* Noise energy level in each band                          */
    SKP_int32   NL[ VAD_N_BANDS ];              
    /* Inverse noise energy level in each band                  */
    SKP_int32   inv_NL[ VAD_N_BANDS ];          
    /* Noise level estimator bias/offset                        */
    SKP_int32   NoiseLevelBias[ VAD_N_BANDS ];  
    /* Frame counter used in the initial phase                  */
    SKP_int32   counter;                        
} SKP_Silk_VAD_state;

/*******************************/
/* Range encoder/decoder state */
/*******************************/
typedef struct {
    SKP_int32   bufferLength;
    SKP_int32   bufferIx;
    SKP_uint32  base_Q32;
    SKP_uint32  range_Q16;
    SKP_int32   error;
    /* Buffer containing payload                                */
    SKP_uint8   buffer[ MAX_ARITHM_BYTES ];     
} SKP_Silk_range_coder_state;

/* Input frequency range detection struct */
typedef struct {
    /* HP filter State */
    SKP_int32                   S_HP_8_kHz[ NB_SOS ][ 2 ];  
    
    SKP_int32                   ConsecSmplsAboveThres;
    /* Accumulated time with active speech */
    SKP_int32                   ActiveSpeech_ms;            
    /* Flag to indicate SWB input */
    SKP_int                     SWB_detected;               
    /* Flag to indicate WB input */
    SKP_int                     WB_detected;                
} SKP_Silk_detect_SWB_state;

#if SWITCH_TRANSITION_FILTERING
/* Variable cut-off low-pass filter state */
typedef struct {
    /* Low pass filter state */
    SKP_int32                   In_LP_State[ 2 ];           
    /* Counter which is mapped to a cut-off frequency */
    SKP_int32                   transition_frame_no;        
    /* Operating mode, 0: switch down, 1: switch up */
    SKP_int                     mode;                       
} SKP_Silk_LP_state;
#endif

/* Structure for one stage of MSVQ */
typedef struct {
    const SKP_int32             nVectors;
    const SKP_int16             *CB_NLSF_Q15;
    const SKP_int16             *Rates_Q5;
} SKP_Silk_NLSF_CBS;

/* Structure containing NLSF MSVQ codebook */
typedef struct {
    const SKP_int32             nStages;

    /* Fields for (de)quantizing */
    const SKP_Silk_NLSF_CBS     *CBStages;
    const SKP_int               *NDeltaMin_Q15;

    /* Fields for arithmetic (de)coding */
    const SKP_uint16            *CDF;
    const SKP_uint16 * const    *StartPtr;
    const SKP_int               *MiddleIx;
} SKP_Silk_NLSF_CB_struct;

/********************************/
/* Encoder state                */
/********************************/
typedef struct {
    /* Range coder state                                                    */
    SKP_Silk_range_coder_state      sRC;                            
    /* Range coder state (for low bitrate redundancy)                       */
    SKP_Silk_range_coder_state      sRC_LBRR;                       
    /* Noise Shape Quantizer State                                          */
    SKP_Silk_nsq_state              sNSQ;                           
    /* Noise Shape Quantizer State ( for low bitrate redundancy )           */
    SKP_Silk_nsq_state              sNSQ_LBRR;                      

#if HIGH_PASS_INPUT
    /* High pass filter state                                               */
    SKP_int32                       In_HP_State[ 2 ];               
#endif
#if SWITCH_TRANSITION_FILTERING
    /* Low pass filter state */
    SKP_Silk_LP_state               sLP;                            
#endif
    /* Voice activity detector state                                        */
    SKP_Silk_VAD_state              sVAD;                           

    SKP_int                         LBRRprevLastGainIndex;
    SKP_int                         prev_sigtype;
    /* Previous signal type and quantization offset                         */
    SKP_int                         typeOffsetPrev;                 
    SKP_int                         prevLag;
    SKP_int                         prev_lagIndex;
    /* API sampling frequency (Hz)                                          */
    SKP_int32                       API_fs_Hz;                      
    /* Previous API sampling frequency (Hz)                                 */
    SKP_int32                       prev_API_fs_Hz;                 
    /* Maximum internal sampling frequency (kHz)                            */
    SKP_int                         maxInternal_fs_kHz;             
    /* Internal sampling frequency (kHz)                                    */
    SKP_int                         fs_kHz;                         
    /* Did we switch yet?                                                   */
    SKP_int                         fs_kHz_changed;                 
    /* Frame length (samples)                                               */
    SKP_int                         frame_length;                   
    /* Subframe length (samples)                                            */
    SKP_int                         subfr_length;                   
    /* Look-ahead for pitch analysis (samples)                              */
    SKP_int                         la_pitch;                       
    /* Look-ahead for noise shape analysis (samples)                        */
    SKP_int                         la_shape;                       
    /* Window length for noise shape analysis (samples)                     */
    SKP_int                         shapeWinLength;                 
    /* Target bitrate (bps)                                                 */
    SKP_int32                       TargetRate_bps;                 
    /* Number of milliseconds to put in each packet                         */
    SKP_int                         PacketSize_ms;                  
    /* Packet loss rate measured by farend                                  */
    SKP_int                         PacketLoss_perc;                
    
    SKP_int32                       frameCounter;
    /* Complexity setting: 0-> low; 1-> medium; 2->high                     */
    SKP_int                         Complexity;                     
    /* Number of states in delayed decision quantization                    */
    SKP_int                         nStatesDelayedDecision;         
    /* Flag for using NLSF interpolation                                    */
    SKP_int                         useInterpolatedNLSFs;           
    /* Filter order for noise shaping filters                               */
    SKP_int                         shapingLPCOrder;                
    /* Filter order for prediction filters                                  */
    SKP_int                         predictLPCOrder;                
    /* Complexity level for pitch estimator                                 */
    SKP_int                         pitchEstimationComplexity;      
    /* Whitening filter order for pitch estimator                           */
    SKP_int                         pitchEstimationLPCOrder;        
    /* Threshold for pitch estimator                                        */
    SKP_int32                       pitchEstimationThreshold_Q16;   
    /* Flag for low complexity LTP quantization                             */
    SKP_int                         LTPQuantLowComplexity;          
    /* Number of survivors in NLSF MSVQ                                     */
    SKP_int                         NLSF_MSVQ_Survivors;            
    /* Flag for deactivating NLSF interp. and fluc. reduction after resets  */
    SKP_int                         first_frame_after_reset;        
    /* Flag for ensuring codec_control only runs once per packet            */
    SKP_int                         controlled_since_last_payload;  
    /* Warping parameter for warped noise shaping                           */
    SKP_int                         warping_Q16;                    

    /* Input/output buffering */
    /* buffer containin input signal                                        */
    SKP_int16                       inputBuf[ MAX_FRAME_LENGTH ];   
    
    SKP_int                         inputBufIx;
    /* number of frames sitting in outputBuf                                */
    SKP_int                         nFramesInPayloadBuf;            
    /* number of bytes sitting in outputBuf                                 */
    SKP_int                         nBytesInPayloadBuf;             

    /* Parameters For LTP scaling Control */
    SKP_int                         frames_since_onset;
    /* Pointers to voiced/unvoiced NLSF codebooks */
    const SKP_Silk_NLSF_CB_struct   *psNLSF_CB[ 2 ];                

    /* Struct for Inband LBRR */ 
    SKP_SILK_LBRR_struct            LBRR_buffer[ MAX_LBRR_DELAY ];
    SKP_int                         oldest_LBRR_idx;
    /* Saves the API setting for query                                      */
    SKP_int                         useInBandFEC;                   
    SKP_int                         LBRR_enabled;
    /* Number of shifts to Gains to get LBRR rate Voiced frames             */
    SKP_int                         LBRR_GainIncreases;             

    /* Bitrate control */
    /* Accumulated diff. between the target bitrate and the switch bitrates */
    SKP_int32                       bitrateDiff;                    
    /* Threshold for switching to a higher internal sample frequency        */
    SKP_int32                       bitrate_threshold_up;           
    /* Threshold for switching to a lower internal sample frequency         */
    SKP_int32                       bitrate_threshold_down;         

    SKP_Silk_resampler_state_struct  resampler_state;

    /* DTX */
    /* Counts concecutive nonactive frames, used by DTX                     */
    SKP_int                         noSpeechCounter;                
    /* Flag to enable DTX                                                   */
    SKP_int                         useDTX;                         
    /* Flag to signal DTX period                                            */
    SKP_int                         inDTX;                         
    /* Flag to indicate Voice Activity                                      */
    SKP_int                         vadFlag;                        

    /* Struct for detecting SWB input */
    SKP_Silk_detect_SWB_state       sSWBdetect;


    /* Buffers */
    /* pulse signal buffer */
    SKP_int8                        q[ MAX_FRAME_LENGTH ];      
    /* pulse signal buffer */
    SKP_int8                        q_LBRR[ MAX_FRAME_LENGTH ]; 

} SKP_Silk_encoder_state;


/************************/
/* Encoder control      */
/************************/
typedef struct {
    /* Quantization indices */
    SKP_int     lagIndex;
    SKP_int     contourIndex;
    SKP_int     PERIndex;
    SKP_int     LTPIndex[ NB_SUBFR ];
    SKP_int     NLSFIndices[ NLSF_MSVQ_MAX_CB_STAGES ];  /* NLSF path of quantized LSF vector   */
    SKP_int     NLSFInterpCoef_Q2;
    SKP_int     GainsIndices[ NB_SUBFR ];
    SKP_int32   Seed;
    SKP_int     LTP_scaleIndex;
    SKP_int     RateLevelIndex;
    SKP_int     QuantOffsetType;
    SKP_int     sigtype;

    /* Prediction and coding parameters */
    SKP_int     pitchL[ NB_SUBFR ];

    SKP_int     LBRR_usage;                     /* Low bitrate redundancy usage                             */
} SKP_Silk_encoder_control;

/* Struct for Packet Loss Concealment */
typedef struct {
    SKP_int32   pitchL_Q8;                      /* Pitch lag to use for voiced concealment                  */
    SKP_int16   LTPCoef_Q14[ LTP_ORDER ];       /* LTP coeficients to use for voiced concealment            */
    SKP_int16   prevLPC_Q12[ MAX_LPC_ORDER ];
    SKP_int     last_frame_lost;                /* Was previous frame lost                                  */
    SKP_int32   rand_seed;                      /* Seed for unvoiced signal generation                      */
    SKP_int16   randScale_Q14;                  /* Scaling of unvoiced random signal                        */
    SKP_int32   conc_energy;
    SKP_int     conc_energy_shift;
    SKP_int16   prevLTP_scale_Q14;
    SKP_int32   prevGain_Q16[ NB_SUBFR ];
    SKP_int     fs_kHz;
} SKP_Silk_PLC_struct;

/* Struct for CNG */
typedef struct {
    SKP_int32   CNG_exc_buf_Q10[ MAX_FRAME_LENGTH ];
    SKP_int     CNG_smth_NLSF_Q15[ MAX_LPC_ORDER ];
    SKP_int32   CNG_synth_state[ MAX_LPC_ORDER ];
    SKP_int32   CNG_smth_Gain_Q16;
    SKP_int32   rand_seed;
    SKP_int     fs_kHz;
} SKP_Silk_CNG_struct;

/********************************/
/* Decoder state                */
/********************************/
typedef struct {
    SKP_Silk_range_coder_state  sRC;                            /* Range coder state                                                    */
    SKP_int32       prev_inv_gain_Q16;
    SKP_int32       sLTP_Q16[ 2 * MAX_FRAME_LENGTH ];
    SKP_int32       sLPC_Q14[ MAX_FRAME_LENGTH / NB_SUBFR + MAX_LPC_ORDER ];
    SKP_int32       exc_Q10[ MAX_FRAME_LENGTH ];
    SKP_int32       res_Q10[ MAX_FRAME_LENGTH ];
    SKP_int16       outBuf[ 2 * MAX_FRAME_LENGTH ];             /* Buffer for output signal                                             */
    SKP_int         lagPrev;                                    /* Previous Lag                                                         */
    SKP_int         LastGainIndex;                              /* Previous gain index                                                  */
    SKP_int         LastGainIndex_EnhLayer;                     /* Previous gain index                                                  */
    SKP_int         typeOffsetPrev;                             /* Previous signal type and quantization offset                         */
    SKP_int32       HPState[ DEC_HP_ORDER ];                    /* HP filter state                                                      */
    const SKP_int16 *HP_A;                                      /* HP filter AR coefficients                                            */
    const SKP_int16 *HP_B;                                      /* HP filter MA coefficients                                            */
    SKP_int         fs_kHz;                                     /* Sampling frequency in kHz                                            */
    SKP_int32       prev_API_sampleRate;                        /* Previous API sample frequency (Hz)                                   */
    SKP_int         frame_length;                               /* Frame length (samples)                                               */
    SKP_int         subfr_length;                               /* Subframe length (samples)                                            */
    SKP_int         LPC_order;                                  /* LPC order                                                            */
    SKP_int         prevNLSF_Q15[ MAX_LPC_ORDER ];              /* Used to interpolate LSFs                                             */
    SKP_int         first_frame_after_reset;                    /* Flag for deactivating NLSF interp. and fluc. reduction after resets  */

    /* For buffering payload in case of more frames per packet */
    SKP_int         nBytesLeft;
    SKP_int         nFramesDecoded;
    SKP_int         nFramesInPacket;
    SKP_int         moreInternalDecoderFrames;
    SKP_int         FrameTermination;

    SKP_Silk_resampler_state_struct  resampler_state;

    const SKP_Silk_NLSF_CB_struct *psNLSF_CB[ 2 ];      /* Pointers to voiced/unvoiced NLSF codebooks */

    /* Parameters used to investigate if inband FEC is used */
    SKP_int         vadFlag;
    SKP_int         no_FEC_counter;                             /* Counts number of frames wo inband FEC                                */
    SKP_int         inband_FEC_offset;                          /* 0: no FEC, 1: FEC with 1 packet offset, 2: FEC w 2 packets offset    */ 

    /* CNG state */
    SKP_Silk_CNG_struct sCNG;

    /* Stuff used for PLC */
    SKP_int         lossCnt;
    SKP_int         prev_sigtype;                               /* Previous sigtype                                                     */

    SKP_Silk_PLC_struct sPLC;



} SKP_Silk_decoder_state;

/************************/
/* Decoder control      */
/************************/
typedef struct {
    /* prediction and coding parameters */
    SKP_int             pitchL[ NB_SUBFR ];
    SKP_int32           Gains_Q16[ NB_SUBFR ];
    SKP_int32           Seed;
    /* holds interpolated and final coefficients, 4-byte aligned */
    SKP_DWORD_ALIGN SKP_int16 PredCoef_Q12[ 2 ][ MAX_LPC_ORDER ];
    SKP_int16           LTPCoef_Q14[ LTP_ORDER * NB_SUBFR ];
    SKP_int             LTP_scale_Q14;

    /* quantization indices */
    SKP_int             PERIndex;
    SKP_int             RateLevelIndex;
    SKP_int             QuantOffsetType;
    SKP_int             sigtype;
    SKP_int             NLSFInterpCoef_Q2;
} SKP_Silk_decoder_control;

#ifdef __cplusplus
}
#endif

#endif
