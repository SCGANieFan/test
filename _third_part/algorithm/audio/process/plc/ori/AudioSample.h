#ifndef __AUDIO_SAMPLE_H__
#define __AUDIO_SAMPLE_H__

#include<stdint.h>

namespace PlcCom {
	class AudioSample
	{
	public:
		AudioSample();
		~AudioSample();

	public:

	private:

	public:
		uint8_t& operator[](int32_t sample);
	public:
		int32_t GetSizeInByte();
		int32_t GetSamples();
		int16_t GetWidth();
		int16_t GetFixPointNum();
		int32_t GetValidSamples();
		int16_t GetChannels();


		bool Init(uint8_t* buff, int16_t width, int16_t channels, int32_t samples, int16_t fixPointNum = 0);
		bool Move(int32_t moveSample);
		bool ProductWithEverySamlpe(AudioSample& src, int32_t productSample);
		bool LeftShift(int16_t bits, bool isSigned);
		bool RightShift(int16_t bits, bool isSigned);
		void Clear();
		bool Append(uint8_t* src, int32_t srcSample);
		bool AppendInFixPoint(int32_t num, int32_t den);
		bool OverlapAdd(uint8_t* src, int32_t overlapSample);
		void used(int32_t usedSample);



	private:
		uint8_t* _buff;
		int16_t _width;
		int16_t _channels;
		int16_t _fixPointNum;
		int32_t _samples;
		int32_t _startSamples;
		int32_t _validSamples;
		int32_t _sizeInByte;
	};
}


#endif