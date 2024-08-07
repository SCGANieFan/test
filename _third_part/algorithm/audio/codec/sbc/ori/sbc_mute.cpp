#include "sbc_com.h"
#include "sbc_priv.h"
#include <memory.h>
int sbc_mute_frame(sbc_info_t*info,void*buff,int max){
	int size = sbc_get_vlc_size(info);
	if(size>max)
		return 0;
	memset(buff,0,size);
	uint8_t* ptr = (uint8_t*)buff;

	if (info->sbcMode == Sbc_Mode_E::SBC_MODE_MSBC)
	{
		*ptr++ = MSBC_SYNC_WORD;
	}
	else if (info->sbcMode == Sbc_Mode_E::SBC_MODE_GSBC)
	{
		*ptr++ = GSBC_SYNC_WORD;
	}
	else {
		uint8_t sync_word = SBC_SYNC_WORD;
		uint8_t numblocks = info->numBlocks/4-1;
		if(info->sbcMode == Sbc_Mode_E::SBC_MODE_ASBC)
		{
			sync_word = ASBC_SYNC_WORD;
			switch(info->numBlocks){
			case ASBC_BLOCKS_15:
				numblocks = 0;
				break;
			case ASBC_BLOCKS_20:
				numblocks = 1;
				break;
			case ASBC_BLOCKS_30:
				numblocks = 2;
				break;
			case ASBC_BLOCKS_60:
				numblocks = 3;
				break;
			default:
				break;
			}
		}
		uint8_t t= (info->sampleFreq << 6)
		| (numblocks<<4)
		| (info->channelMode << 2)
		| (info->allocMethod << 1)
		| (info->numSubBands == 8?1:0)
		;
		*ptr++=sync_word;
		*ptr++=t;
		*ptr++=info->bitPool;
	}
	sbc_crc_calc(info,buff);
	return size;
}

