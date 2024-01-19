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

/*
 * File Name:   SKP_Silk_VAD.c
 * Description: Silk VAD.
 */

#include <stdlib.h>
#include "SKP_Silk_main.h"

/**********************************/
/* Initialization of the Silk VAD */
/**********************************/
SKP_int SKP_Silk_VAD_Init(                              /* O    Return value, 0 if success                  */ 
    SKP_Silk_VAD_state              *psSilk_VAD         /* I/O  Pointer to Silk VAD state                   */ 
)
{
    SKP_int b, ret = 0;

    /* reset state memory */
    SKP_memset( psSilk_VAD, 0, sizeof( SKP_Silk_VAD_state ) );

    /* init noise levels */
    /* Initialize array with approx pink noise levels (psd proportional to inverse of frequency) */
    for( b = 0; b < VAD_N_BANDS; b++ ) {
        psSilk_VAD->NoiseLevelBias[ b ] = SKP_max_32( SKP_DIV32_16( VAD_NOISE_LEVELS_BIAS, b + 1 ), 1 );
    }

    /* Initialize state */
    for( b = 0; b < VAD_N_BANDS; b++ ) {
        psSilk_VAD->NL[ b ]     = SKP_MUL( 100, psSilk_VAD->NoiseLevelBias[ b ] );
        psSilk_VAD->inv_NL[ b ] = SKP_DIV32( SKP_int32_MAX, psSilk_VAD->NL[ b ] );
    }
    psSilk_VAD->counter = 15;

    /* init smoothed energy-to-noise ratio*/
    for( b = 0; b < VAD_N_BANDS; b++ ) {
        psSilk_VAD->NrgRatioSmth_Q8[ b ] = 100 * 256;       /* 100 * 256 --> 20 dB SNR */
    }

    return( ret );
}

/* Weighting factors for tilt measure */
    //117 23 -46 -46
const static SKP_int32 tiltWeights[ VAD_N_BANDS ] = { 30000, 6000, -12000, -12000 };

/***************************************/
/* Get the speech activity level in Q8 */
/***************************************/
//
SKP_int SKP_Silk_VAD_GetSA_Q8(                /* O    Return value, 0 if success      */                      
    /* I/O  Silk VAD state                  */
    SKP_Silk_VAD_state              *psSilk_VAD,                    
    /* O    Speech activity level in Q8     */
    SKP_int                         *pSA_Q8,                        
    /* O    SNR for current frame in Q7     */
    SKP_int                         *pSNR_dB_Q7,                    
    /* O    Smoothed SNR for each band      */
    SKP_int                         pQuality_Q15[ VAD_N_BANDS ],    
    /* O    current frame's frequency tilt  */
    SKP_int                         *pTilt_Q15,                     
    /* I    PCM input       [framelength]   */
    const SKP_int16                 pIn[],                         
    /* I    Input frame length              */
    const SKP_int                   framelength                     
)
{
    SKP_int   SA_Q15, input_tilt;
    SKP_int32 scratch[ 3 * MAX_FRAME_LENGTH / 2 ];
    SKP_int   decimated_framelength, dec_subframe_length, dec_subframe_offset, SNR_Q7, i, b, s;
    SKP_int32 sumSquared, smooth_coef_Q16;
    SKP_int16 HPstateTmp;

    SKP_int16 X[ VAD_N_BANDS ][ MAX_FRAME_LENGTH / 2 ];
    SKP_int32 Xnrg[ VAD_N_BANDS ];
    SKP_int32 NrgToNoiseRatio_Q8[ VAD_N_BANDS ];
    SKP_int32 speech_nrg, x_tmp;
    SKP_int   ret = 0;

    /* Safety checks */
    //安全检查
    SKP_assert( VAD_N_BANDS == 4 );
    SKP_assert( MAX_FRAME_LENGTH >= framelength );
    SKP_assert( framelength <= 512 );

    /***********************/
    /* Filter and Decimate */
    /***********************/
    //滤波与抽取

    /* 0-8 kHz to 0-4 kHz and 4-8 kHz */
    //
    SKP_Silk_ana_filt_bank_1( pIn,          &psSilk_VAD->AnaState[  0 ], 
                            &X[ 0 ][ 0 ], &X[ 3 ][ 0 ], &scratch[ 0 ], framelength ); 
    /* 0-4 kHz to 0-2 kHz and 2-4 kHz */
    //
    SKP_Silk_ana_filt_bank_1( &X[ 0 ][ 0 ], &psSilk_VAD->AnaState1[ 0 ], 
                            &X[ 0 ][ 0 ], &X[ 2 ][ 0 ], &scratch[ 0 ], SKP_RSHIFT( framelength, 1 ) );
    /* 0-2 kHz to 0-1 kHz and 1-2 kHz */
    //
    SKP_Silk_ana_filt_bank_1( &X[ 0 ][ 0 ], &psSilk_VAD->AnaState2[ 0 ], 
                            &X[ 0 ][ 0 ], &X[ 1 ][ 0 ], &scratch[ 0 ], SKP_RSHIFT( framelength, 2 ) );



    /*********************************************/
    /* HP filter on lowest band (differentiator) */
    /*********************************************/
    //最低带 HP滤波器(微分器)
    decimated_framelength = SKP_RSHIFT( framelength, 3 );
    X[ 0 ][ decimated_framelength - 1 ] = SKP_RSHIFT( X[ 0 ][ decimated_framelength - 1 ], 1 );
    HPstateTmp = X[ 0 ][ decimated_framelength - 1 ];
    for( i = decimated_framelength - 1; i > 0; i-- ) {
        X[ 0 ][ i - 1 ]  = SKP_RSHIFT( X[ 0 ][ i - 1 ], 1 );
        X[ 0 ][ i ]     -= X[ 0 ][ i - 1 ];
    }
    X[ 0 ][ 0 ] -= psSilk_VAD->HPstate;
    psSilk_VAD->HPstate = HPstateTmp;

    /*************************************/
    /* Calculate the energy in each band */
    /*************************************/
    //计算每个波段的能量Xnrg[b]
    for( b = 0; b < VAD_N_BANDS; b++ ) {        
        /* Find the decimated framelength in the non-uniformly divided bands */
        //求非均匀分割波段的十进制帧长
        decimated_framelength = SKP_RSHIFT( framelength, SKP_min_int( VAD_N_BANDS - b, VAD_N_BANDS - 1 ) );

        /* Split length into subframe lengths */
        //将长度分割为子帧长度
        dec_subframe_length = SKP_RSHIFT( decimated_framelength, VAD_INTERNAL_SUBFRAMES_LOG2 );
        dec_subframe_offset = 0;

        /* Compute energy per sub-frame */
        //计算每子帧的能量
        /* initialize with summed energy of last subframe */
        //初始化与最后一个子帧的总和能量 
        Xnrg[ b ] = psSilk_VAD->XnrgSubfr[ b ];
        for( s = 0; s < VAD_INTERNAL_SUBFRAMES; s++ ) 
        {
            sumSquared = 0;
            for( i = 0; i < dec_subframe_length; i++ )
            {
                /* The energy will be less than dec_subframe_length * ( SKP_int16_MIN / 8 ) ^ 2.            */
                //
                /* Therefore we can accumulate with no risk of overflow (unless dec_subframe_length > 128)  */
                //
                x_tmp = SKP_RSHIFT( X[ b ][ i + dec_subframe_offset ], 3 );
                sumSquared = SKP_SMLABB( sumSquared, x_tmp, x_tmp );

                /* Safety check */
                //
                SKP_assert( sumSquared >= 0 );
            }

            /* Add/saturate summed energy of current subframe */
            //添加/饱和 当前子帧的总和能量
            if( s < VAD_INTERNAL_SUBFRAMES - 1 )
            {
                //Xnrg[ b ]+sumSquared的最高位为1？ 2^(31)-1:(a)+(b)
                Xnrg[ b ] = SKP_ADD_POS_SAT32( Xnrg[ b ], sumSquared );
            } 
            else 
            {
                /* Look-ahead subframe */
                //
                Xnrg[ b ] = SKP_ADD_POS_SAT32( Xnrg[ b ], SKP_RSHIFT( sumSquared, 1 ) );
            }

            dec_subframe_offset += dec_subframe_length;
        }
        psSilk_VAD->XnrgSubfr[ b ] = sumSquared; 
    }

    /********************/
    /* Noise estimation */
    /********************/
    //噪声估计
    //根据子带能量Xnrg 估计 噪声能量水平psSilk_VAD->NL 
    SKP_Silk_VAD_GetNoiseLevels( &Xnrg[ 0 ], psSilk_VAD );

    /***********************************************/
    /* Signal-plus-noise to noise ratio estimation */
    /***********************************************/
    //
    sumSquared = 0;
    input_tilt = 0;
    for( b = 0; b < VAD_N_BANDS; b++ )
    {
        //speech_nrg语音能量 = Xnrg子带能量 - NL噪声能量
        speech_nrg = Xnrg[ b ] - psSilk_VAD->NL[ b ];
        //若语音能量大于0
        if( speech_nrg > 0 ) 
        {
            
            /* Divide, with sufficient resolution */
            //NrgToNoiseRatio = Xnrg/NL
            if( ( Xnrg[ b ] & 0xFF800000 ) == 0 ) 
            {
                //若子带能量较小
                //NrgToNoiseRatio_Q8 = Xnrg<<8 / (NL+1)
                NrgToNoiseRatio_Q8[ b ] = SKP_DIV32( SKP_LSHIFT( Xnrg[ b ], 8 ), psSilk_VAD->NL[ b ] + 1 );
            } 
            else 
            {   //若子带能量较大
                //NrgToNoiseRatio_Q8 = Xnrg/(NL>>8 + 1)
                NrgToNoiseRatio_Q8[ b ] = SKP_DIV32( Xnrg[ b ], SKP_RSHIFT( psSilk_VAD->NL[ b ], 8 ) + 1 );
            }

            /* Convert to log domain */
            //SNR_Q7 = log(NrgToNoiseRatio_Q8)-8*128
            //SNR = log(NrgToNoiseRatio)
            SNR_Q7 = SKP_Silk_lin2log( NrgToNoiseRatio_Q8[ b ] ) - 8 * 128;

            /* Sum-of-squares */
            //sumSquared += + SNR_Q7^2
            sumSquared = SKP_SMLABB( sumSquared, SNR_Q7, SNR_Q7 );          /* Q14 */

            /* Tilt measure */
            if( speech_nrg < ( 1 << 20 ) ) 
            {
                /* Scale down SNR value for small subband speech energies */
                
                //语音能量 speech_nrg 趋近于 0
                //SNR_Q7 = sqrt(speech_nrg)<<6 * SNR_Q7
                //SNR = sqrt(speech_nrg)*SNR
                SNR_Q7 = SKP_SMULWB( SKP_LSHIFT( SKP_Silk_SQRT_APPROX( speech_nrg ), 6 ), SNR_Q7 );
            }
            //input_tilt = input_tilt + tiltWeights*SNR_Q7
            input_tilt = SKP_SMLAWB( input_tilt, tiltWeights[ b ], SNR_Q7 );
        } 
        else 
        {
            NrgToNoiseRatio_Q8[ b ] = 256;
        }
    }

    /* Mean-of-squares */
    //sumSquared / VAD_N_BANDS
    sumSquared = SKP_DIV32_16( sumSquared, VAD_N_BANDS ); /* Q14 */

    /* Root-mean-square approximation, scale to dBs, and write to output pointer */
    //pSNR_dB_Q7 = sqrt(sumSquared)
    *pSNR_dB_Q7 = ( SKP_int16 )( 3 * SKP_Silk_SQRT_APPROX( sumSquared ) );  /* Q7 */

    /*********************************/
    /* Speech Probability Estimation */
    /*********************************/
    //SA_Q15 = sigm(VAD_SNR_FACTOR_Q16*pSNR_dB_Q7-VAD_NEGATIVE_OFFSET_Q5)
    //SA = sigm(VAD_SNR_FACTOR*pSNR_dB - VAD_NEGATIVE_OFFSET)
    SA_Q15 = SKP_Silk_sigm_Q15( SKP_SMULWB( VAD_SNR_FACTOR_Q16, *pSNR_dB_Q7 ) - VAD_NEGATIVE_OFFSET_Q5 );

    /**************************/
    /* Frequency Tilt Measure */
    /**************************/
    //pTilt_Q15 = (sigm(input_tilt)-16384)<<1
    *pTilt_Q15 = SKP_LSHIFT( SKP_Silk_sigm_Q15( input_tilt ) - 16384, 1 );

    /**************************************************/
    /* Scale the sigmoid output based on power levels */
    /**************************************************/

    speech_nrg = 0;
    for( b = 0; b < VAD_N_BANDS; b++ ) 
    {
        /* Accumulate signal-without-noise energies, higher frequency bands have more weight */
        //speech_nrg = speech_nrg + (b+1)(Xnrg-NL)>>4；
        speech_nrg += ( b + 1 ) * SKP_RSHIFT( Xnrg[ b ] - psSilk_VAD->NL[ b ], 4 );
    }

    /* Power scaling */
    //
    if( speech_nrg <= 0 ) 
    {
        //SA_Q15 = SA_Q15>>1
        SA_Q15 = SKP_RSHIFT( SA_Q15, 1 ); 
    } 
    else if( speech_nrg < 32768 ) 
    {
        /* square-root */
        speech_nrg = SKP_Silk_SQRT_APPROX( SKP_LSHIFT( speech_nrg, 15 ) );
        //SA_Q15 = SA_Q15*speech_nrg
        SA_Q15 = SKP_SMULWB( 32768 + speech_nrg, SA_Q15 ); 
    }

    /* Copy the resulting speech activity in Q8 to *pSA_Q8 */
    //pSA_Q8 = SA_Q15
    *pSA_Q8 = SKP_min_int( SKP_RSHIFT( SA_Q15, 7 ), SKP_uint8_MAX );

    /***********************************/
    /* Energy Level and SNR estimation */
    /***********************************/
    //
    /* Smoothing coefficient */
    //smooth_coef_Q16 = VAD_SNR_SMOOTH_COEF_Q18*SA_Q15^2
    smooth_coef_Q16 = SKP_SMULWB( VAD_SNR_SMOOTH_COEF_Q18, SKP_SMULWB( SA_Q15, SA_Q15 ) );
    for( b = 0; b < VAD_N_BANDS; b++ ) 
    {
        /* compute smoothed energy-to-noise ratio per band */
        //NrgRatioSmth_Q8 = NrgRatioSmth_Q8 + (NrgToNoiseRatio_Q8 - NrgRatioSmth_Q8)*smooth_coef_Q16
        //NrgRatioSmth_Q8低通滤波
        psSilk_VAD->NrgRatioSmth_Q8[ b ] = SKP_SMLAWB( psSilk_VAD->NrgRatioSmth_Q8[ b ], 
            NrgToNoiseRatio_Q8[ b ] - psSilk_VAD->NrgRatioSmth_Q8[ b ], smooth_coef_Q16 );

        /* signal to noise ratio in dB per band */
        // SNR_Q7 = 3*log(NrgRatioSmth_Q8)
        SNR_Q7 = 3 * ( SKP_Silk_lin2log( psSilk_VAD->NrgRatioSmth_Q8[b] ) - 8 * 128 );
        /* quality = sigmoid( 0.25 * ( SNR_dB - 16 ) ); */
        //pQuality_Q15 = sigm((SNR_Q7 - 16 * 128)>>4)
        pQuality_Q15[ b ] = SKP_Silk_sigm_Q15( SKP_RSHIFT( SNR_Q7 - 16 * 128, 4 ) );
    }

    return( ret );
}

/**************************/
/* Noise level estimation */
/**************************/
//更新 子带能量
//psSilk_VAD->NL[ k ] 
//psSilk_VAD->inv_NL[ k ]
void SKP_Silk_VAD_GetNoiseLevels(
    const SKP_int32                 pX[ VAD_N_BANDS ],  /* I    subband energies                            */
    SKP_Silk_VAD_state              *psSilk_VAD         /* I/O  Pointer to Silk VAD state                   */ 
)
{
    SKP_int   k;
    SKP_int32 nl, nrg, inv_nrg;
    SKP_int   coef, min_coef;

    /* Initially faster smoothing */
    
    if( psSilk_VAD->counter < 1000 )
    { 
        /* 1000 = 20 sec */
        //前20s内，进入此选择
        //SKP_int16_MAX = 2^(15)-1
        //min_coef = SKP_int16_MAX/(counter>>4+1)
        min_coef = SKP_DIV32_16( SKP_int16_MAX, SKP_RSHIFT( psSilk_VAD->counter, 4 ) + 1 );  
    } 
    else 
    {
        min_coef = 0;
    }

    for( k = 0; k < VAD_N_BANDS; k++ ) 
    {
        /* Get old noise level estimate for current band */
        //
        nl = psSilk_VAD->NL[ k ];
        SKP_assert( nl >= 0 );
        
        /* Add bias */
        //nrg = nrg + NoiseLevelBias
        nrg = SKP_ADD_POS_SAT32( pX[ k ], psSilk_VAD->NoiseLevelBias[ k ] ); 
        SKP_assert( nrg > 0 );
        
        /* Invert energies */
        //inv_nrg = SKP_int32_MAX / nrg
        inv_nrg = SKP_DIV32( SKP_int32_MAX, nrg );
        SKP_assert( inv_nrg >= 0 );
        
        /* Less update when subband energy is high */
        //
        //nrg > 8*nl
        if( nrg > SKP_LSHIFT( nl, 3 ) ) 
        {
            coef = VAD_NOISE_LEVEL_SMOOTH_COEF_Q16 >> 3;
        } 
        //nrg < nl
        else if( nrg < nl ) 
        {
            coef = VAD_NOISE_LEVEL_SMOOTH_COEF_Q16;
        } 
        //nl < nrg < 8*nl
        else 
        {
            //coef = inv_nrg*nl*VAD_NOISE_LEVEL_SMOOTH_COEF_Q16*2
            coef = SKP_SMULWB( SKP_SMULWW( inv_nrg, nl ), VAD_NOISE_LEVEL_SMOOTH_COEF_Q16 << 1 );
        }

        /* Initially faster smoothing */
        //coef最小值限定在min_coef
        coef = SKP_max_int( coef, min_coef );

        /* Smooth inverse energies */
        //噪声能量倒数 低通滤波
        //inv_NL = inv_NL + （inv_nrg-inv_NL）*coef
        psSilk_VAD->inv_NL[ k ] = SKP_SMLAWB( psSilk_VAD->inv_NL[ k ], 
                                            inv_nrg - psSilk_VAD->inv_NL[ k ], coef );
        SKP_assert( psSilk_VAD->inv_NL[ k ] >= 0 );

        /* Compute noise level by inverting again */
        //nl = nrg
        nl = SKP_DIV32( SKP_int32_MAX, psSilk_VAD->inv_NL[ k ] );
        SKP_assert( nl >= 0 );

        /* Limit noise levels (guarantee 7 bits of head room) */
        //nl = nrg 最大值限定在0x00FFFFFF
        nl = SKP_min( nl, 0x00FFFFFF );

        /* Store as part of state */
        //
        psSilk_VAD->NL[ k ] = nl;
    }

    /* Increment frame counter */
    //
    psSilk_VAD->counter++;
}
