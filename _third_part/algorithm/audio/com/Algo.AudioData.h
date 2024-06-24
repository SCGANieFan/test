#pragma once

#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.Data.h"



namespace Algo {
	namespace Audio {
		class AudioInfo
		{
		public:
			AudioInfo() {};
			~AudioInfo() {};
		public:
			INLINE b1 Init(i32 rate, i16 width, i16 channels) {
				_rate = rate;
				_width = width;
				_channels = channels;
				_bytesPerSample = _width * _channels;
				return true;
			};
		public:
			i32 _rate = 0;
			i16 _width = 0;
			i16 _channels = 0;
			i32 _bytesPerSample = 0;
		};

		class AudioData :protected Data
		{
		public:
			AudioData() {};
			~AudioData() {};
		public:
			INLINE b1 Init(const AudioInfo* pInfo, Buffer* buffer) {
				AudioData::Data::Init(buffer);
				_info = pInfo;
				return true;
			}
		public:
			const AudioInfo* _info = 0;
		};
	}
}





