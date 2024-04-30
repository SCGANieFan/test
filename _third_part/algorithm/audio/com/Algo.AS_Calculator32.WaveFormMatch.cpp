#include"Algo.AS_Calculator32.h"
i16 AS_Calculator32::WaveFormMatch(AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample)
{
	i64 matchFactor = 0;
	i32 matchFactorOpt = 0;
	i16 lagOpt = 0;

	i32* pTemplOri = (i32*)&dst[dstSample];
	i32* pCompOri = (i32*)&cmp[cmpSample];
	i32* pTempl;
	i32* pComp;

	for (i16 s = 0; s < seekSample; s++) {
		matchFactor = 0;
		pTempl = pTemplOri + s * dst._info->_channels;
		pComp = pCompOri;
		for (i16 m = 0; m < matchSample; m++) {
			for (i16 ch = 0; ch < dst._info->_channels; ch++) {
				matchFactor += ((i64)(*pTempl) * (*pComp)) >> 8;
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

