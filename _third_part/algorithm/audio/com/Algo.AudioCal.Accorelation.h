#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioData.h"
#if 1

namespace Algo {
	namespace Audio {
		class Accorelation_c
		{
		private:
			typedef i32 (*Accorelation_CB)(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample);
		public:
			Accorelation_c() {};
			~Accorelation_c() {};
		public:
			void Init(i16 width, i16 channels);
			INLINE i32 DoCrossCorr(void* ref, void* cmp, i32 seekSample, i32 matchSample){
				return _Accorelation(ref,cmp,_channels,seekSample,matchSample);
			}

		private:
			Accorelation_CB _Accorelation;
			i16 _channels;
		};

	}
}


#endif