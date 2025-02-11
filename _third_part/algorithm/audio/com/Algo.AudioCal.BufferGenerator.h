#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
namespace Algo {
	namespace Audio {

		template<class T>
		struct BufferGenerator_t {
			STATIC INLINE void GenerateLine(T* factor, i32 samples, i16 fpNum = 0) {
				i64 samplesInner = samples + 1;
				for (i32 i = 0; i < samples; i++) {
					i64 num = samplesInner - 1 - i;
					factor[i] = (T)((num << fpNum) / samplesInner);
				}
			}
			STATIC INLINE void GenerateCos(T* factor, i32 samples, i16 fpNum = 0) {
				STATIC const i32 _PI_Q15 = 102943;
				STATIC const i32 _0p1_Q15 = 690;
				i32 PI_QFp = _PI_Q15 >> (15 - fpNum);
				i32 _0P1_QFp = _0p1_Q15 >> (15 - fpNum);
				i32 ONE_QFp = ((i32)1 << fpNum);
				i32 ONE_QFpP1 = ((i32)1 << (fpNum + 1));
				for (i32 i = 0; i < (samples + 1) / 2; i++) {
					i32 xQFp = PI_QFp * i / samples;
					i32 offset = _0P1_QFp * i / samples;
					i32 xHat2QFpPFp = xQFp * xQFp;
					i32 xHat2QFpP2 = (xHat2QFpPFp + ONE_QFpP1) >> (fpNum + 2);
					i32 xHat2QFp = (xQFp * xQFp) >> fpNum;
					i32 xHat4QFp = (xHat2QFp * xHat2QFp) >> fpNum;
					factor[i] = (T)(ONE_QFp - xHat2QFpP2 + xHat4QFp / 48 - offset);
				}
				i32 xx = ((samples + 1) / 2 * 2) - 1;
				for (i32 i = (samples + 1) / 2; i < samples; i++)
				{
					factor[i] = ONE_QFp - factor[xx - i];
				}
			}
		};

		template<>
		struct BufferGenerator_t<f32> {
			STATIC INLINE void GenerateLine(f32* factor, i32 samples, i16 fpNum = 0) {
				f32 samplesInner = samples + 1;
				for (i32 i = 0; i < samples; i++) {
					f32 num = samplesInner - 1 - i;
					factor[i] = num / samplesInner;
				}
			}
			STATIC INLINE void GenerateCos(f32* factor, i32 samples, i16 fpNum = 0) {
				STATIC const f32 _PI = 3.1415926f;
				for (i32 i = 0; i < (samples + 1) / 2; i++) {
					f32 x = _PI * i / samples;
					f32 xHat2 = x * x;
					f32 xHat2D4 = xHat2 / 4;
					f32 xHat4D48 = xHat2 * xHat2 / 48;
					factor[i] = (f32)((f32)1 - xHat2D4 + xHat4D48);
				}
				i32 xx = ((samples + 1) / 2 * 2) - 1;
				for (i32 i = (samples + 1) / 2; i < samples; i++)
				{
					factor[i] = 1 - factor[xx - i];
				}
			}
		};

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
		public:
			template<class T>
			STATIC INLINE void Generate(BufferChoose_e choose, T* factor, i32 samples, i16 fpNum = 0) {
				fpNum = TypeIdentify_c::IsFloat<T>() ? 0 : MIN(fpNum, 15);
				if (choose == BufferChoose_e::WINDOW_LINE_FADE) {
					BufferGenerator_t<T>::GenerateLine(factor, samples, fpNum);
				}
				else if (choose == BufferChoose_e::WINDOW_COSINE_FADE) {
					BufferGenerator_t<T>::GenerateCos(factor, samples, fpNum);
				}
			}
		};
	}
}

#endif