#pragma once

#include "sbc_com.h"


#if 1
//sampleFreq
enum SbcSampleRate_e{
	SBC_SAMPLE_FREQ_16K=0,
	SBC_SAMPLE_FREQ_32K=1,
	SBC_SAMPLE_FREQ_44K1=1,
	SBC_SAMPLE_FREQ_48K=3,
	MSBC_SAMPLE_FREQ_16K=0,
	GSBC_SAMPLE_FREQ_48K=0,
	ASBC_SAMPLE_FREQ_32K = 0,
	ASBC_SAMPLE_FREQ_48K = 1,
	ASBC_SAMPLE_FREQ_88K2 = 2,
	ASBC_SAMPLE_FREQ_96K = 3,
};

enum SbcChannelMode_e {
	SBC_CHNL_MODE_MONO_E = 0,
	SBC_CHNL_MODE_DUAL_CHNL_E = 1,
	SBC_CHNL_MODE_STEREO_E = 2,
	SBC_CHNL_MODE_JOINT_STEREO_E = 3,
};

enum SbcAllocMethod_e {
	SBC_ALLOC_METHOD_LOUDNESS_E =0,
	SBC_ALLOC_METHOD_SNR_E =1,
};


#endif

//EXTERNC

/** get the size of sbc encoder instance
 * @return: return the size of sbc encoder instance
 */
EXTERNC int sbc_encoder_get_size();

/** reset the sbc encoder
 * @param handle[in]:the encoder instance
 * @param info[in]:set encoder paramer
 */
EXTERNC bool sbc_encoder_init(void*handle, sbc_info_t* info);


/** sbc encode process
 *@param handle[in]: the encoder instance
 *@param pcm[in]: the input pcm
 *@param output[in]: the vlc output buff
 *@param max[in]: the vlc output buff size
 *@return: the vlc size
 */
EXTERNC int sbc_encode(void *handle, pcm_s16_t *pcm, void *output, int max);

/** sbc get info from encoder handle
 * @param handle[in]:the encoder instance
 * @param info[out]
 */
EXTERNC void sbc_encoder_get_info(void*handle, sbc_info_t*info);

/** sbc filter 8band analysis
* @param handle[in]:the encoder instance
* @param PcmData[in]
*/
EXTERNC void sbc_filter_8band_analysis(void* handle, pcm_s16_t* PcmData);

/** sbc convert bps to bitPool, should use after sbc_encoder_init()
* @param handle[in]:the encoder instance
* @param bps[in]
* @param bitPool[out]
*/
EXTERNC int sbc_bps2bitPool(void* handle, int bps);

/** sbc convert bitPool to bps, should use after sbc_encoder_init()
* @param handle[in]:the encoder instance
* @param bps[out]
*/
EXTERNC int sbc_bitPool2bps(void* handle);
