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

#include "SKP_Silk_main_FIX.h"
#include "SKP_Silk_tuning_parameters.h"

/* Compute gain to make warped filter coefficients have a zero mean log frequency response on a     */
/* non-warped frequency scale. (So that it can be implemented with a minimum-phase monic filter.)   */
SKP_INLINE SKP_int32 warped_gain( // gain in Q16
    const SKP_int32     *coefs_Q24, 
    SKP_int             lambda_Q16, 
    SKP_int             order 
) {
    SKP_int   i;
    SKP_int32 gain_Q24;

    lambda_Q16 = -lambda_Q16;
    gain_Q24 = coefs_Q24[ order - 1 ];
    for( i = order - 2; i >= 0; i-- ) {
        gain_Q24 = SKP_SMLAWB( coefs_Q24[ i ], gain_Q24, lambda_Q16 );
    }
    gain_Q24  = SKP_SMLAWB( SKP_FIX_CONST( 1.0, 24 ), gain_Q24, -lambda_Q16 );
    return SKP_INVERSE32_varQ( gain_Q24, 40 );
}

/* Convert warped filter coefficients to monic pseudo-warped coefficients and limit maximum     */
/* amplitude of monic warped coefficients by using bandwidth expansion on the true coefficients */
SKP_INLINE void limit_warped_coefs( 
    SKP_int32           *coefs_syn_Q24,
    SKP_int32           *coefs_ana_Q24,
    SKP_int             lambda_Q16,
    SKP_int32           limit_Q24,
    SKP_int             order
) {
    SKP_int   i, iter, ind = 0;
    SKP_int32 tmp, maxabs_Q24, chirp_Q16, gain_syn_Q16, gain_ana_Q16;
    SKP_int32 nom_Q16, den_Q24;

    /* Convert to monic coefficients */
    lambda_Q16 = -lambda_Q16;
    for( i = order - 1; i > 0; i-- ) {
        coefs_syn_Q24[ i - 1 ] = SKP_SMLAWB( coefs_syn_Q24[ i - 1 ], coefs_syn_Q24[ i ], lambda_Q16 );
        coefs_ana_Q24[ i - 1 ] = SKP_SMLAWB( coefs_ana_Q24[ i - 1 ], coefs_ana_Q24[ i ], lambda_Q16 );
    }
    lambda_Q16 = -lambda_Q16;
    nom_Q16  = SKP_SMLAWB( SKP_FIX_CONST( 1.0, 16 ), -lambda_Q16,        lambda_Q16 );
    den_Q24  = SKP_SMLAWB( SKP_FIX_CONST( 1.0, 24 ), coefs_syn_Q24[ 0 ], lambda_Q16 );
    gain_syn_Q16 = SKP_DIV32_varQ( nom_Q16, den_Q24, 24 );
    den_Q24  = SKP_SMLAWB( SKP_FIX_CONST( 1.0, 24 ), coefs_ana_Q24[ 0 ], lambda_Q16 );
    gain_ana_Q16 = SKP_DIV32_varQ( nom_Q16, den_Q24, 24 );
    for( i = 0; i < order; i++ ) {
        coefs_syn_Q24[ i ] = SKP_SMULWW( gain_syn_Q16, coefs_syn_Q24[ i ] );
        coefs_ana_Q24[ i ] = SKP_SMULWW( gain_ana_Q16, coefs_ana_Q24[ i ] );
    }

    for( iter = 0; iter < 10; iter++ ) {
        /* Find maximum absolute value */
        maxabs_Q24 = -1;
        for( i = 0; i < order; i++ ) {
            tmp = SKP_max( SKP_abs_int32( coefs_syn_Q24[ i ] ), SKP_abs_int32( coefs_ana_Q24[ i ] ) );
            if( tmp > maxabs_Q24 ) {
                maxabs_Q24 = tmp;
                ind = i;
            }
        }
        if( maxabs_Q24 <= limit_Q24 ) {
            /* Coefficients are within range - done */
            return;
        }

        /* Convert back to true warped coefficients */
        for( i = 1; i < order; i++ ) {
            coefs_syn_Q24[ i - 1 ] = SKP_SMLAWB( coefs_syn_Q24[ i - 1 ], coefs_syn_Q24[ i ], lambda_Q16 );
            coefs_ana_Q24[ i - 1 ] = SKP_SMLAWB( coefs_ana_Q24[ i - 1 ], coefs_ana_Q24[ i ], lambda_Q16 );
        }
        gain_syn_Q16 = SKP_INVERSE32_varQ( gain_syn_Q16, 32 );
        gain_ana_Q16 = SKP_INVERSE32_varQ( gain_ana_Q16, 32 );
        for( i = 0; i < order; i++ ) {
            coefs_syn_Q24[ i ] = SKP_SMULWW( gain_syn_Q16, coefs_syn_Q24[ i ] );
            coefs_ana_Q24[ i ] = SKP_SMULWW( gain_ana_Q16, coefs_ana_Q24[ i ] );
        }

        /* Apply bandwidth expansion */
        chirp_Q16 = SKP_FIX_CONST( 0.99, 16 ) - SKP_DIV32_varQ(
            SKP_SMULWB( maxabs_Q24 - limit_Q24, SKP_SMLABB( SKP_FIX_CONST( 0.8, 10 ), SKP_FIX_CONST( 0.1, 10 ), iter ) ), 
            SKP_MUL( maxabs_Q24, ind + 1 ), 22 );
        SKP_Silk_bwexpander_32( coefs_syn_Q24, order, chirp_Q16 );
        SKP_Silk_bwexpander_32( coefs_ana_Q24, order, chirp_Q16 );

        /* Convert to monic warped coefficients */
        lambda_Q16 = -lambda_Q16;
        for( i = order - 1; i > 0; i-- ) {
            coefs_syn_Q24[ i - 1 ] = SKP_SMLAWB( coefs_syn_Q24[ i - 1 ], coefs_syn_Q24[ i ], lambda_Q16 );
            coefs_ana_Q24[ i - 1 ] = SKP_SMLAWB( coefs_ana_Q24[ i - 1 ], coefs_ana_Q24[ i ], lambda_Q16 );
        }
        lambda_Q16 = -lambda_Q16;
        nom_Q16  = SKP_SMLAWB( SKP_FIX_CONST( 1.0, 16 ), -lambda_Q16,        lambda_Q16 );
        den_Q24  = SKP_SMLAWB( SKP_FIX_CONST( 1.0, 24 ), coefs_syn_Q24[ 0 ], lambda_Q16 );
        gain_syn_Q16 = SKP_DIV32_varQ( nom_Q16, den_Q24, 24 );
        den_Q24  = SKP_SMLAWB( SKP_FIX_CONST( 1.0, 24 ), coefs_ana_Q24[ 0 ], lambda_Q16 );
        gain_ana_Q16 = SKP_DIV32_varQ( nom_Q16, den_Q24, 24 );
        for( i = 0; i < order; i++ ) {
            coefs_syn_Q24[ i ] = SKP_SMULWW( gain_syn_Q16, coefs_syn_Q24[ i ] );
            coefs_ana_Q24[ i ] = SKP_SMULWW( gain_ana_Q16, coefs_ana_Q24[ i ] );
        }
    }
	SKP_assert( 0 );
}

/**************************************************************/
/* Compute noise shaping coefficients and initial gain values */
/**************************************************************/
//计算噪声整形系数 和 初始增益值


void SKP_Silk_noise_shape_analysis_FIX(
    /* I/O  Encoder state FIX                           */
    SKP_Silk_encoder_state_FIX      *psEnc,         
    /* I/O  Encoder control FIX                         */
    SKP_Silk_encoder_control_FIX    *psEncCtrl,     
    /* I    LPC residual from pitch analysis            */
    const SKP_int16                 *pitch_res,     
    /* I    Input signal [ frame_length + la_shape ]    */
    const SKP_int16                 *x              
)
{
    SKP_Silk_shape_state_FIX *psShapeSt = &psEnc->sShape;
    SKP_int     k, i, nSamples, Qnrg, b_Q14, warping_Q16, scale = 0;
    SKP_int32   SNR_adj_dB_Q7, HarmBoost_Q16, HarmShapeGain_Q16, Tilt_Q16, tmp32;
    SKP_int32   nrg, pre_nrg_Q30, log_energy_Q7, log_energy_prev_Q7, energy_variation_Q7;
    SKP_int32   delta_Q16, BWExp1_Q16, BWExp2_Q16, gain_mult_Q16, gain_add_Q16, strength_Q16, b_Q8;
    SKP_int32   auto_corr[     MAX_SHAPE_LPC_ORDER + 1 ];
    SKP_int32   refl_coef_Q16[ MAX_SHAPE_LPC_ORDER ];
    SKP_int32   AR1_Q24[       MAX_SHAPE_LPC_ORDER ];
    SKP_int32   AR2_Q24[       MAX_SHAPE_LPC_ORDER ];
    SKP_int16   x_windowed[    SHAPE_LPC_WIN_MAX ];
    const SKP_int16 *x_ptr, *pitch_res_ptr;

    /* Point to start of first LPC analysis block */
    x_ptr = x - psEnc->sCmn.la_shape;

    /****************/
    /* CONTROL SNR  */
    /****************/
    /* Reduce SNR_dB values if recent bitstream has exceeded TargetRate */
    //current_SNR_dB = SNR_dB - BufferedInChannel_ms*0.05
    psEncCtrl->current_SNR_dB_Q7 = psEnc->SNR_dB_Q7 
                                - SKP_SMULWB(SKP_LSHIFT( ( SKP_int32 )psEnc->BufferedInChannel_ms, 7 ), 
                                            SKP_FIX_CONST( 0.05, 16 ) );

    /* Reduce SNR_dB if inband FEC used */
    if( psEnc->speech_activity_Q8 > SKP_FIX_CONST( LBRR_SPEECH_ACTIVITY_THRES, 8 ) ) 
    {
        //current_SNR = current_SNR - inBandFEC_SNR_comp
        psEncCtrl->current_SNR_dB_Q7 -= SKP_RSHIFT( psEnc->inBandFEC_SNR_comp_Q8, 1 );
    }

    /****************/
    /* GAIN CONTROL */
    /****************/
    /* Input quality is the average of the quality in the lowest two VAD bands */
    //input_quality = （input_quality_bands[0] + input_quality_bands[1]）/2
    psEncCtrl->input_quality_Q14 = ( SKP_int )SKP_RSHIFT( 
                ( SKP_int32 )psEncCtrl->input_quality_bands_Q15[ 0 ] 
                + psEncCtrl->input_quality_bands_Q15[ 1 ], 2 );

    /* Coding quality level, between 0.0_Q0 and 1.0_Q0, but in Q14 */
    //coding_quality = sigm(current_SNR_dB - 18.0)
    psEncCtrl->coding_quality_Q14 = SKP_RSHIFT(
                                                SKP_Silk_sigm_Q15( 
                                                    SKP_RSHIFT_ROUND( 
                                                        psEncCtrl->current_SNR_dB_Q7 
                                                        - SKP_FIX_CONST( 18.0, 7 )
                                                        , 4 )
                                                    ), 
                                                1 );

    /* Reduce coding SNR during low speech activity */
    //b = (1 - speech_activity )^2
    b_Q8 = SKP_FIX_CONST( 1.0, 8 ) - psEnc->speech_activity_Q8;
    b_Q8 = SKP_SMULWB( SKP_LSHIFT( b_Q8, 8 ), b_Q8 );
    /*
    SNR_adj_dB = current_SNR_dB
                    + (-BG_SNR_DECR_dB)*b 
                    * (1 + input_quality)*coding_quality
    */
    SNR_adj_dB_Q7 = SKP_SMLAWB( 
        psEncCtrl->current_SNR_dB_Q7,
        SKP_SMULBB( SKP_FIX_CONST( -BG_SNR_DECR_dB, 7 ) >> ( 4 + 1 ), 
                    b_Q8 ),                                // Q11
        SKP_SMULWB( SKP_FIX_CONST( 1.0, 14 ) + psEncCtrl->input_quality_Q14, 
                    psEncCtrl->coding_quality_Q14 ) );     // Q12

    if( psEncCtrl->sCmn.sigtype == SIG_TYPE_VOICED ) {
        /* Reduce gains for periodic signals */
        //SNR_adj_dB = SNR_adj_dB + HARM_SNR_INCR_dB*LTPCorr
        SNR_adj_dB_Q7 = SKP_SMLAWB( SNR_adj_dB_Q7, SKP_FIX_CONST( HARM_SNR_INCR_dB, 8 ), psEnc->LTPCorr_Q15 );
    } 
    else
    { 
        /* For unvoiced signals and low-quality input, adjust the quality slower than SNR_dB setting */
        //SNR_adj_dB = SNR_adj_dB + (6.0 + (-0.4)*current_SNR_dB) * （1.0 - input_quality）
        SNR_adj_dB_Q7 = SKP_SMLAWB( SNR_adj_dB_Q7, 
            SKP_SMLAWB( SKP_FIX_CONST( 6.0, 9 ), -SKP_FIX_CONST( 0.4, 18 ), psEncCtrl->current_SNR_dB_Q7 ),
            SKP_FIX_CONST( 1.0, 14 ) - psEncCtrl->input_quality_Q14 );
    }

    /*************************/
    /* SPARSENESS PROCESSING */
    /*************************/
    /* Set quantizer offset */
    if( psEncCtrl->sCmn.sigtype == SIG_TYPE_VOICED )
    {
        /* Initally set to 0; may be overruled in process_gains(..) */
        psEncCtrl->sCmn.QuantOffsetType = 0;
        psEncCtrl->sparseness_Q8 = 0;
    } 
    else 
    {
        /* Sparseness measure, based on relative fluctuations of energy per 2 milliseconds */
        nSamples = SKP_LSHIFT( psEnc->sCmn.fs_kHz, 1 );
        energy_variation_Q7 = 0;
        log_energy_prev_Q7  = 0;
        pitch_res_ptr = pitch_res;
        for( k = 0; k < FRAME_LENGTH_MS / 2; k++ ) 
        {    
            SKP_Silk_sum_sqr_shift( &nrg, &scale, pitch_res_ptr, nSamples );
            nrg += SKP_RSHIFT( nSamples, scale );           // Q(-scale)
            
            log_energy_Q7 = SKP_Silk_lin2log( nrg );
            if( k > 0 ) {
                //energy_variation = log_energy - log_energy_prev
                energy_variation_Q7 += SKP_abs( log_energy_Q7 - log_energy_prev_Q7 );
            }
            log_energy_prev_Q7 = log_energy_Q7;
            pitch_res_ptr += nSamples;
        }
        //sparseness = sigm((energy_variation - 5) * 0.1)
        psEncCtrl->sparseness_Q8 = SKP_RSHIFT(
            SKP_Silk_sigm_Q15( 
                SKP_SMULWB( 
                    energy_variation_Q7 - SKP_FIX_CONST( 5.0, 7 ), 
                    SKP_FIX_CONST( 0.1, 16 )) 
                ), 
            7 );

        /* Set quantization offset depending on sparseness measure */
        if( psEncCtrl->sparseness_Q8 > SKP_FIX_CONST( SPARSENESS_THRESHOLD_QNT_OFFSET, 8 ) ) {
            psEncCtrl->sCmn.QuantOffsetType = 0;
        } else {
            psEncCtrl->sCmn.QuantOffsetType = 1;
        }
        
        /* Increase coding SNR for sparse signals */
        //SNR_adj_dB = SNR_adj_dB + SPARSE_SNR_INCR_dB * (sparseness - 0.5)
        SNR_adj_dB_Q7 = SKP_SMLAWB( SNR_adj_dB_Q7, SKP_FIX_CONST( SPARSE_SNR_INCR_dB, 15 ), psEncCtrl->sparseness_Q8 - SKP_FIX_CONST( 0.5, 8 ) );
    }

    /*******************************/
    /* Control bandwidth expansion */
    /*******************************/
    /* More BWE for signals with high prediction gain */
    //strength = predGain * FIND_PITCH_WHITE_NOISE_FRACTION
    strength_Q16 = SKP_SMULWB( psEncCtrl->predGain_Q16, 
                                SKP_FIX_CONST( FIND_PITCH_WHITE_NOISE_FRACTION, 16 ) );
    //BWExp1 = BWExp2 = BANDWIDTH_EXPANSION/(1+strength^2)
    BWExp1_Q16 = BWExp2_Q16 = SKP_DIV32_varQ( 
        SKP_FIX_CONST( BANDWIDTH_EXPANSION, 16 ), 
        SKP_SMLAWW( SKP_FIX_CONST( 1.0, 16 ),strength_Q16, strength_Q16 ),
        16 );
    //delta = (1 - 0.75*coding_quality) * LOW_RATE_BANDWIDTH_EXPANSION_DELTA
    delta_Q16  = SKP_SMULWB( SKP_FIX_CONST( 1.0, 16 ) 
                            - SKP_SMULBB( 3, psEncCtrl->coding_quality_Q14 ), 
                            SKP_FIX_CONST( LOW_RATE_BANDWIDTH_EXPANSION_DELTA, 16 ) );
    //BWExp1 = BWExp1 - delta
    //BWExp2 = BWExp2 + delta
    BWExp1_Q16 = SKP_SUB32( BWExp1_Q16, delta_Q16 );
    BWExp2_Q16 = SKP_ADD32( BWExp2_Q16, delta_Q16 );
    /* BWExp1 will be applied after BWExp2, so make it relative */
    //BWExp1 = BWExp1 / BWExp2
    BWExp1_Q16 = SKP_DIV32_16( SKP_LSHIFT( BWExp1_Q16, 14 ), SKP_RSHIFT( BWExp2_Q16, 2 ) );

    if( psEnc->sCmn.warping_Q16 > 0 )
    {
        /* Slightly more warping in analysis will move 
        quantization noise up in frequency, where it's better masked */
        //warping = warping + coding_quality* 0.01
        warping_Q16 = SKP_SMLAWB( psEnc->sCmn.warping_Q16, 
                                psEncCtrl->coding_quality_Q14, 
                                SKP_FIX_CONST( 0.01, 18 ) );
    } 
    else 
    {
        warping_Q16 = 0;
    }

    /********************************************/
    /* Compute noise shaping AR coefs and gains */
    /********************************************/
    for( k = 0; k < NB_SUBFR; k++ )
    {
        /* Apply window: sine slope followed by flat part followed by cosine slope */
        //正弦斜率，然后是平坦部分，然后是余弦斜率
        SKP_int shift, slope_part, flat_part;
        //flat_part 5ms samples
        flat_part = psEnc->sCmn.fs_kHz * 5;
        //slope_part (shapeWinLength - flat_part)/2
        //
        slope_part = SKP_RSHIFT( psEnc->sCmn.shapeWinLength - flat_part, 1 );

        //x_ptr  -->  sine wimdow  -->x_windowed
        SKP_Silk_apply_sine_window( x_windowed, x_ptr, 1, slope_part );
        shift = slope_part;
        SKP_memcpy( x_windowed + shift, x_ptr + shift, flat_part * sizeof(SKP_int16) );
        shift += flat_part;
        //(x_ptr + slope_part + flat_part) -->  cos wimdow  -->  （x_windowed+ slope_part + flat_part）
        SKP_Silk_apply_sine_window( x_windowed + shift, x_ptr + shift, 2, slope_part );
        
        /* Update pointer: next LPC analysis block */
        x_ptr += psEnc->sCmn.subfr_length;

        if( psEnc->sCmn.warping_Q16 > 0 )
        {
            /* Calculate warped auto correlation */
            SKP_Silk_warped_autocorrelation_FIX( auto_corr, &scale, x_windowed, 
                warping_Q16, psEnc->sCmn.shapeWinLength, psEnc->sCmn.shapingLPCOrder ); 
        } 
        else
        {
            /* Calculate regular auto correlation */
            SKP_Silk_autocorr( auto_corr, &scale, x_windowed, 
                psEnc->sCmn.shapeWinLength, psEnc->sCmn.shapingLPCOrder + 1 );
        }

        /* Add white noise, as a fraction of energy */
        auto_corr[0] = SKP_ADD32( auto_corr[0], SKP_max_32( SKP_SMULWB( SKP_RSHIFT( auto_corr[ 0 ], 4 ), 
            SKP_FIX_CONST( SHAPE_WHITE_NOISE_FRACTION, 20 ) ), 1 ) ); 

        /* Calculate the reflection coefficients using schur */
        nrg = SKP_Silk_schur64( refl_coef_Q16, auto_corr, psEnc->sCmn.shapingLPCOrder );
        SKP_assert( nrg >= 0 );

        /* Convert reflection coefficients to prediction coefficients */
        SKP_Silk_k2a_Q16( AR2_Q24, refl_coef_Q16, psEnc->sCmn.shapingLPCOrder );

        Qnrg = -scale;          // range: -12...30
        SKP_assert( Qnrg >= -12 );
        SKP_assert( Qnrg <=  30 );

        /* Make sure that Qnrg is an even number */
        if( Qnrg & 1 ) {
            Qnrg -= 1;
            nrg >>= 1;
        }

        tmp32 = SKP_Silk_SQRT_APPROX( nrg );
        Qnrg >>= 1;             // range: -6...15
        //Gains = sqrt(nrg)
        psEncCtrl->Gains_Q16[ k ] = SKP_LSHIFT_SAT32( tmp32, 16 - Qnrg );

        if( psEnc->sCmn.warping_Q16 > 0 ) 
        {
            /* Adjust gain for warping */
            gain_mult_Q16 = warped_gain( AR2_Q24, warping_Q16, 
                                        psEnc->sCmn.shapingLPCOrder );
            SKP_assert( psEncCtrl->Gains_Q16[ k ] >= 0 );
            //Gains = Gains * gain_mult
            psEncCtrl->Gains_Q16[ k ] = SKP_SMULWW( psEncCtrl->Gains_Q16[ k ], 
                                                    gain_mult_Q16 );
            if( psEncCtrl->Gains_Q16[ k ] < 0 ) 
            {
                psEncCtrl->Gains_Q16[ k ] = SKP_int32_MAX;
            }
        }

        /* Bandwidth expansion for synthesis filter shaping */
        SKP_Silk_bwexpander_32( AR2_Q24, psEnc->sCmn.shapingLPCOrder,
                                BWExp2_Q16 );

        /* Compute noise shaping filter coefficients */
        SKP_memcpy( AR1_Q24, AR2_Q24, 
                    psEnc->sCmn.shapingLPCOrder * sizeof( SKP_int32 ) );

        /* Bandwidth expansion for analysis filter shaping */
        SKP_assert( BWExp1_Q16 <= SKP_FIX_CONST( 1.0, 16 ) );
        SKP_Silk_bwexpander_32( AR1_Q24, psEnc->sCmn.shapingLPCOrder, 
                                BWExp1_Q16 );

        /* Ratio of prediction gains, in energy domain */
        SKP_Silk_LPC_inverse_pred_gain_Q24( &pre_nrg_Q30, AR2_Q24, 
                                            psEnc->sCmn.shapingLPCOrder );
        SKP_Silk_LPC_inverse_pred_gain_Q24( &nrg,         AR1_Q24, 
                                            psEnc->sCmn.shapingLPCOrder );

        //psEncCtrl->GainsPre[ k ] = 1.0f - 0.7f * ( 1.0f - pre_nrg / nrg ) = 0.3f + 0.7f * pre_nrg / nrg;
        //pre_nrg = (0.7*pre_nrg)*2
        //GainsPre = 0.3 + pre_nrg/nrg
        pre_nrg_Q30 = SKP_LSHIFT32( SKP_SMULWB( pre_nrg_Q30, SKP_FIX_CONST( 0.7, 15 ) ), 1 );
        psEncCtrl->GainsPre_Q14[ k ] = ( SKP_int ) SKP_FIX_CONST( 0.3, 14 ) 
                                        + SKP_DIV32_varQ( pre_nrg_Q30, nrg, 14 );

        /* Convert to monic warped prediction coefficients and limit absolute values */
        limit_warped_coefs( AR2_Q24, AR1_Q24, warping_Q16,
                            SKP_FIX_CONST( 3.999, 24 ), 
                            psEnc->sCmn.shapingLPCOrder );

        /* Convert from Q24 to Q13 and store in int16 */
        for( i = 0; i < psEnc->sCmn.shapingLPCOrder; i++ ) {
            psEncCtrl->AR1_Q13[ k * MAX_SHAPE_LPC_ORDER + i ] 
                = (SKP_int16)SKP_SAT16( SKP_RSHIFT_ROUND( AR1_Q24[ i ], 11 ) );
            psEncCtrl->AR2_Q13[ k * MAX_SHAPE_LPC_ORDER + i ] 
                = (SKP_int16)SKP_SAT16( SKP_RSHIFT_ROUND( AR2_Q24[ i ], 11 ) );
        }
    }

    /*****************/
    /* Gain tweaking */
    /*****************/
    /* Increase gains during low speech activity and put lower limit on gains */
    //gain_mult = 10^(-(-16+SNR_adj_dB*0.16))
    //gain_mult = 10^(16-SNR_adj_dB*0.16)
    gain_mult_Q16 = SKP_Silk_log2lin( 
                                    -SKP_SMLAWB( 
                                        -SKP_FIX_CONST( 16.0, 7 ), 
                                        SNR_adj_dB_Q7,                            
                                        SKP_FIX_CONST( 0.16, 16 ))
                                    );
    //gain_add = 10^(16+NOISE_FLOOR_dB*0.16)
    gain_add_Q16  = SKP_Silk_log2lin(  
                                    SKP_SMLAWB(  
                                        SKP_FIX_CONST( 16.0, 7 ), 
                                        SKP_FIX_CONST( NOISE_FLOOR_dB, 7 ),       
                                        SKP_FIX_CONST( 0.16, 16 ) ) 
                                    );
    //tmp32 = 10^(16+RELATIVE_MIN_GAIN_dB*0.16)*avgGain
    tmp32         = SKP_Silk_log2lin(  
                                    SKP_SMLAWB( 
                                        SKP_FIX_CONST( 16.0, 7 ), 
                                        SKP_FIX_CONST( RELATIVE_MIN_GAIN_dB, 7 ), 
                                        SKP_FIX_CONST( 0.16, 16 ) ) 
                                    );
    tmp32 = SKP_SMULWW( psEnc->avgGain_Q16, tmp32 );
    //gain_add = gain_add + 10^(16-RELATIVE_MIN_GAIN_dB*0.16)*avgGain
    gain_add_Q16 = SKP_ADD_SAT32( gain_add_Q16, tmp32 );
    SKP_assert( gain_mult_Q16 >= 0 );

    for( k = 0; k < NB_SUBFR; k++ ) {
        //Gains = Gains*gain_mult
        psEncCtrl->Gains_Q16[ k ] = SKP_SMULWW( psEncCtrl->Gains_Q16[ k ], gain_mult_Q16 );
        if( psEncCtrl->Gains_Q16[ k ] < 0 ) {
            psEncCtrl->Gains_Q16[ k ] = SKP_int32_MAX;
        }
    }

    for( k = 0; k < NB_SUBFR; k++ ) {
        //Gains = Gains+gain_add
        psEncCtrl->Gains_Q16[ k ] = SKP_ADD_POS_SAT32( psEncCtrl->Gains_Q16[ k ], gain_add_Q16 );
        //avgGain = avgGain + (Gains - avgGain)*speech_activity*GAIN_SMOOTHING_COEF
        psEnc->avgGain_Q16 = SKP_ADD_SAT32( 
            psEnc->avgGain_Q16, 
            SKP_SMULWB(
                psEncCtrl->Gains_Q16[ k ] - psEnc->avgGain_Q16, 
                SKP_RSHIFT_ROUND( 
                                SKP_SMULBB( psEnc->speech_activity_Q8, 
                                            SKP_FIX_CONST( GAIN_SMOOTHING_COEF, 10 ) ),
                                2 ) 
            ) );
    }

    /************************************************/
    /* Decrease level during fricatives (de-essing) */
    /************************************************/
    //gain_mult = 1 + (INPUT_TILT + coding_quality*HIGH_RATE_INPUT_TILT)
    gain_mult_Q16 = SKP_FIX_CONST( 1.0, 16 ) 
                    + SKP_RSHIFT_ROUND( 
                        SKP_MLA( 
                            SKP_FIX_CONST( INPUT_TILT, 26 ), 
                            psEncCtrl->coding_quality_Q14, 
                            SKP_FIX_CONST( HIGH_RATE_INPUT_TILT, 12 ) ),
                        10 );
    if( psEncCtrl->input_tilt_Q15 <= 0 && psEncCtrl->sCmn.sigtype == SIG_TYPE_UNVOICED ) 
    {
        if( psEnc->sCmn.fs_kHz == 24 )
        {
            //essStrength = -input_tilt*(speech_activity*(1-sparseness))
            SKP_int32 essStrength_Q15 = SKP_SMULWW( 
                                                    -psEncCtrl->input_tilt_Q15, 
                                                    SKP_SMULBB( 
                                                        psEnc->speech_activity_Q8, 
                                                        SKP_FIX_CONST( 1.0, 8 ) 
                                                        - psEncCtrl->sparseness_Q8 )
                                                  );
            //tmp32 = 10^(16  - essStrength*DE_ESSER_COEF_SWB_dB*0.16)
            tmp32 = SKP_Silk_log2lin( 
                                    SKP_FIX_CONST( 16.0, 7 ) 
                                    - SKP_SMULWB( 
                                        essStrength_Q15, 
                                        SKP_SMULWB( 
                                            SKP_FIX_CONST( DE_ESSER_COEF_SWB_dB, 7 ),
                                            SKP_FIX_CONST( 0.16, 17 ) ) 
                                                ) );
            //gain_mult = gain_mult*tmp32
            gain_mult_Q16 = SKP_SMULWW( gain_mult_Q16, tmp32 );
        } 
        else if( psEnc->sCmn.fs_kHz == 16 ) 
        {
            //essStrength = -input_tilt*(speech_activity*(1-sparseness))
            SKP_int32 essStrength_Q15 = SKP_SMULWW(
                                                -psEncCtrl->input_tilt_Q15, 
                                                SKP_SMULBB( 
                                                    psEnc->speech_activity_Q8, 
                                                    SKP_FIX_CONST( 1.0, 8 ) 
                                                    - psEncCtrl->sparseness_Q8 
                                                        ));
            //tmp32 = 10^(16  - essStrength*DE_ESSER_COEF_SWB_dB*0.16)
            tmp32 = SKP_Silk_log2lin( 
                                    SKP_FIX_CONST( 16.0, 7 ) 
                                    - SKP_SMULWB( essStrength_Q15, 
                                    SKP_SMULWB( 
                                        SKP_FIX_CONST( DE_ESSER_COEF_WB_dB, 7 ), 
                                        SKP_FIX_CONST( 0.16, 17 ) 
                                                ) ) );
            gain_mult_Q16 = SKP_SMULWW( gain_mult_Q16, tmp32 );
        }
        else
        {
            SKP_assert( psEnc->sCmn.fs_kHz == 12 || psEnc->sCmn.fs_kHz == 8 );
        }
    }

    for( k = 0; k < NB_SUBFR; k++ )
    {
        //GainsPre = GainsPre*gain_mult
        psEncCtrl->GainsPre_Q14[ k ] = SKP_SMULWB( gain_mult_Q16, psEncCtrl->GainsPre_Q14[ k ] );
    }

    /************************************************/
    /* Control low-frequency shaping and noise tilt */
    /************************************************/
    /* Less low frequency shaping for noisy inputs */
    //strength = LOW_FREQ_SHAPING
    //          *(1 + LOW_QUALITY_LOW_FREQ_SHAPING_DECR * (input_quality_bands-1))
    
    strength_Q16 = SKP_MUL( 
                        SKP_FIX_CONST( LOW_FREQ_SHAPING, 0 ), 
                        SKP_FIX_CONST( 1.0, 16 ) 
                        +SKP_SMULBB( 
                            SKP_FIX_CONST( LOW_QUALITY_LOW_FREQ_SHAPING_DECR, 1 ),
                            psEncCtrl->input_quality_bands_Q15[ 0 ]
                            - SKP_FIX_CONST( 1.0, 15 ) 
                                    ) );

    if( psEncCtrl->sCmn.sigtype == SIG_TYPE_VOICED ) 
    {
        /* Reduce low frequencies quantization noise 
        for periodic signals, depending on pitch lag */
        /*f = 400; 
        freqz([1, -0.98 + 2e-4 * f], [1, -0.97 + 7e-4 * f], 2^12, Fs); 
        axis([0, 1000, -10, 1])*/

        //fs_kHz_inv = 0.2/fs_kHz
        SKP_int fs_kHz_inv = SKP_DIV32_16( SKP_FIX_CONST( 0.2, 14 ), 
                                            psEnc->sCmn.fs_kHz );
        for( k = 0; k < NB_SUBFR; k++ )
        {
            //b = fs_kHz_inv + 3/pitchL
            b_Q14 = fs_kHz_inv 
                    + SKP_DIV32_16( 
                        SKP_FIX_CONST( 3.0, 14 ), 
                        psEncCtrl->sCmn.pitchL[ k ] ); 
            /* Pack two coefficients in one int32 */
            //LF_shp = (1-b-strength) | (b-1)
            psEncCtrl->LF_shp_Q14[ k ]  = SKP_LSHIFT( 
                                                    SKP_FIX_CONST( 1.0, 14 ) 
                                                    - b_Q14 
                                                    - SKP_SMULWB( strength_Q16, b_Q14 ), 
                                                    16 );
            psEncCtrl->LF_shp_Q14[ k ] |= (SKP_uint16)( b_Q14 - SKP_FIX_CONST( 1.0, 14 ) );
        }
        SKP_assert( SKP_FIX_CONST( HARM_HP_NOISE_COEF, 24 ) < SKP_FIX_CONST( 0.5, 24 ) ); // Guarantees that second argument to SMULWB() is within range of an SKP_int16
        //Tilt = -HP_NOISE_COEF - (1 - HP_NOISE_COEF)*(HARM_HP_NOISE_COEF*speech_activity)
        Tilt_Q16 = - SKP_FIX_CONST( HP_NOISE_COEF, 16 ) 
                    - SKP_SMULWB( 
                                SKP_FIX_CONST( 1.0, 16 ) 
                                - SKP_FIX_CONST( HP_NOISE_COEF, 16 ), 
                                SKP_SMULWB( 
                                    SKP_FIX_CONST( HARM_HP_NOISE_COEF, 24 ), 
                                    psEnc->speech_activity_Q8 ) 
                                );
    } 
    else
    {
        //b =  1.3/fs_kHz
        b_Q14 = SKP_DIV32_16( 21299, psEnc->sCmn.fs_kHz ); // 1.3_Q0 = 21299_Q14
        /* Pack two coefficients in one int32 */
       //LF_shp = (1-b-strength*0.6*b) | (b-1)
        psEncCtrl->LF_shp_Q14[ 0 ]  = SKP_LSHIFT( 
                                                SKP_FIX_CONST( 1.0, 14 ) 
                                                - b_Q14 
                                                - SKP_SMULWB(
                                                    strength_Q16, 
                                                    SKP_SMULWB( 
                                                        SKP_FIX_CONST( 0.6, 16 ), 
                                                        b_Q14 ) ),
                                                16 );
        psEncCtrl->LF_shp_Q14[ 0 ] |= (SKP_uint16)( b_Q14 - SKP_FIX_CONST( 1.0, 14 ) );

        for( k = 1; k < NB_SUBFR; k++ )
        {
            //LF_shp = LF_shp[0]
            psEncCtrl->LF_shp_Q14[ k ] = psEncCtrl->LF_shp_Q14[ 0 ];
        }
        //Tilt = HP_NOISE_COEF
        Tilt_Q16 = -SKP_FIX_CONST( HP_NOISE_COEF, 16 );
    }

    /****************************/
    /* HARMONIC SHAPING CONTROL */
    /****************************/
    /* Control boosting of harmonic frequencies */
    //HarmBoost = (1 - coding_quality)*LTPCorr*LOW_RATE_HARMONIC_BOOST
    HarmBoost_Q16 = SKP_SMULWB( 
                                SKP_SMULWB( 
                                    SKP_FIX_CONST( 1.0, 17 ) 
                                    - SKP_LSHIFT( psEncCtrl->coding_quality_Q14, 3 ), 
                                    psEnc->LTPCorr_Q15 ), 
                                SKP_FIX_CONST( LOW_RATE_HARMONIC_BOOST, 16 ) );

    /* More harmonic boost for noisy input signals */
    //HarmBoost = HarmBoost + (1 - input_quality)*LOW_INPUT_QUALITY_HARMONIC_BOOST
    HarmBoost_Q16 = SKP_SMLAWB( 
                            HarmBoost_Q16, 
                            SKP_FIX_CONST( 1.0, 16 ) 
                            - SKP_LSHIFT( psEncCtrl->input_quality_Q14, 2 ), 
                            SKP_FIX_CONST( LOW_INPUT_QUALITY_HARMONIC_BOOST, 16 ) );

    if( USE_HARM_SHAPING && psEncCtrl->sCmn.sigtype == SIG_TYPE_VOICED )
    {
        /* More harmonic noise shaping for high bitrates or noisy input */
        //HarmShapeGain = HARMONIC_SHAPING
        //              +(1-(1-coding_quality)
        //              *input_quality)*HIGH_RATE_OR_LOW_QUALITY_HARMONIC_SHAPING
        HarmShapeGain_Q16 = SKP_SMLAWB( 
                    SKP_FIX_CONST( HARMONIC_SHAPING, 16 ), 
                    SKP_FIX_CONST( 1.0, 16 ) 
                    - SKP_SMULWB( 
                        SKP_FIX_CONST( 1.0, 18 ) 
                        - SKP_LSHIFT( psEncCtrl->coding_quality_Q14, 4 ),
                        psEncCtrl->input_quality_Q14 ), 
                    SKP_FIX_CONST( HIGH_RATE_OR_LOW_QUALITY_HARMONIC_SHAPING, 16 ) );

        /* Less harmonic noise shaping for less periodic signals */
        //HarmShapeGain = HarmShapeGain*LTPCorr
        HarmShapeGain_Q16 = SKP_SMULWB( 
                                        SKP_LSHIFT( HarmShapeGain_Q16, 1 ), 
                                        SKP_Silk_SQRT_APPROX( 
                                            SKP_LSHIFT( psEnc->LTPCorr_Q15, 15 ) 
                                        ) );
    } 
    else
    {
        HarmShapeGain_Q16 = 0;
    }

    /*************************/
    /* Smooth over subframes */
    /*************************/
    for( k = 0; k < NB_SUBFR; k++ ) {
        /*
        HarmBoost_smth      = HarmBoost_smth
                                +(HarmBoost-HarmBoost_smth)*SUBFR_SMTH_COEF

        HarmShapeGain_smth  = HarmShapeGain_smth
                                +(HarmShapeGain-HarmShapeGain_smth)*HarmShapeGain_smth

        Tilt_smth           = Tilt_smth
                                +(Tilt-Tilt_smth)*SUBFR_SMTH_COEF
        */
        psShapeSt->HarmBoost_smth_Q16 = SKP_SMLAWB( 
                                            psShapeSt->HarmBoost_smth_Q16,     
                                            HarmBoost_Q16     
                                            - psShapeSt->HarmBoost_smth_Q16,     
                                            SKP_FIX_CONST( SUBFR_SMTH_COEF, 16 ) );
        psShapeSt->HarmShapeGain_smth_Q16 =SKP_SMLAWB( 
                                            psShapeSt->HarmShapeGain_smth_Q16, 
                                            HarmShapeGain_Q16 
                                            - psShapeSt->HarmShapeGain_smth_Q16, 
                                            SKP_FIX_CONST( SUBFR_SMTH_COEF, 16 ) );
        psShapeSt->Tilt_smth_Q16 =SKP_SMLAWB( 
                                    psShapeSt->Tilt_smth_Q16,          
                                    Tilt_Q16          
                                    - psShapeSt->Tilt_smth_Q16,          
                                    SKP_FIX_CONST( SUBFR_SMTH_COEF, 16 ) );

        psEncCtrl->HarmBoost_Q14[ k ]     = ( SKP_int )SKP_RSHIFT_ROUND( 
                                            psShapeSt->HarmBoost_smth_Q16,     2 );
        psEncCtrl->HarmShapeGain_Q14[ k ] = ( SKP_int )SKP_RSHIFT_ROUND( 
                                            psShapeSt->HarmShapeGain_smth_Q16, 2 );
        psEncCtrl->Tilt_Q14[ k ]          = ( SKP_int )SKP_RSHIFT_ROUND( 
                                            psShapeSt->Tilt_smth_Q16,          2 );
    }
}
