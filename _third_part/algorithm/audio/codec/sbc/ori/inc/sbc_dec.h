#pragma once

#include "sbc_com.h"
//EXTERNC

/** get the size of sbc decoder instance
 * @return: return the size of sbc decoder instance
 */
EXTERNC int sbc_decoder_get_size();

/** reset the sbc decoder
 * @param handle[in]:the decoder instance
 */
EXTERNC bool sbc_decoder_init(void* handle);

/** sbc get info from decoder handle
 * @param handle[in]
 * @param info[out]
 */
EXTERNC void sbc_decoder_get_info(void*handle,sbc_info_t*info);

/** sbc decode process
 *@param handle[in]: the decoder instance
 *@param input[in]: the vlc input data
 *@param size[in]: the vlc input data size
 *@param pcm[out]: the output pcm
 *@return: the input data used size
 */
EXTERNC int sbc_decode(void*handle,void*input,int size,pcm_s16_t*pcm);

/** vlc input data parser
 *@param handle[in]: the decoder instance
 *@param Buff[in]: the vlc input data
 *@param Len[in]: the vlc input data size
 *@param BytesDecoded[out]: the length of used vlc input data when decoding a frame
 *@return: status
 */
EXTERNC char sbc_frames_parser(void *handle, void *Buff, uint16_t Len, uint16_t *BytesDecoded);

/** 8 band synth filter without output, updata the the decoder instance
 *@param handle[in]: the decoder instance
 */
EXTERNC void sbc_8band_synth_filter_without_output(void *handle);












