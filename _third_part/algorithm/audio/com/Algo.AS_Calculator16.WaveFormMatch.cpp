#include"Algo.AS_Calculator16.h"
i16 AS_Calculator16::WaveFormMatch(AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample)
{
	i32 matchFactor = 0;
	i32 matchFactorOpt = 0;
	i16 lagOpt = 0;

	i16* pTemplOri = (i16*)&dst[dstSample];
	i16* pCompOri = (i16*)&cmp[cmpSample];
	i16* pTempl;
	i16* pComp;

	for (i16 s = 0; s < seekSample; s++) {
		matchFactor = 0;
		pTempl = pTemplOri + s * dst._info->_channels;
		pComp = pCompOri;
		for (i16 m = 0; m < matchSample; m++) {
			for (i16 ch = 0; ch < dst._info->_channels; ch++) {
				matchFactor += ((i32)(*pTempl) * (*pComp)) >> 8;
				pTempl++;
				pComp++;
			}
		}
		if (matchFactorOpt < matchFactor)
		{
			matchFactorOpt = matchFactor;
			lagOpt = s;
		}
	}
	return lagOpt;
}

