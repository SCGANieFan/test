#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
namespace Algo {
	namespace Audio {
		class BufferGenerator_c
		{
		public:
			enum class BufferChoose_e {
				WINDOW_LINE_FADE = 0,
				WINDOW_COSINE_FADE,
			};
		public:
			BufferGenerator_c() {};
			~BufferGenerator_c() {};
			template<class T>
			STATIC INLINE void Generate(BufferChoose_e choose, T* factor, i32 samples, i16 fpNum) {
				fpNum = MIN(fpNum, 15);
				//factor 1 --> 0
				if (choose == BufferChoose_e::WINDOW_LINE_FADE) {
					for (i32 i = 0; i < samples; i++) {
						i32 num = samples - i;
						factor[i] = (num << fpNum) / samples;
					}
				}
				else if (choose == BufferChoose_e::WINDOW_COSINE_FADE) {
					i32 PI_QFp = _PI_Q15 >> (15 - fpNum);
					i32 ONE_QFp = ((i32)1 << fpNum);
					for (i32 i = 0; i < (samples + 1) / 2; i++){
						i32 xQFp = PI_QFp * i / samples;
						i32 xHat2QFp = (xQFp * xQFp) >> fpNum;
						i32 xHat4QFp = (xHat2QFp * xHat2QFp) >> fpNum;
						factor[i] = (i16)(ONE_QFp - (xHat2QFp >> 2) + xHat4QFp / 48);
					}
					i32 xx = ((samples + 1) / 2 * 2) - 1;
					for (i32 i = (samples + 1) / 2; i < samples; i++)
					{
						factor[i] = ONE_QFp - factor[xx - i];
					}
				}
			}
		private:
			STATIC const i32 _PI_Q15 = 102943;
		};
	}
}

#endif