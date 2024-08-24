#if 1
#include"MTF.h"
#include <stdio.h>
#define PATH "../../source/video/h265/"

#if 1
#define FILE_NAME "lolTest_24fps_352x352.h265"
#endif

void H265Test()
{
	MultiemdiaTestInit();
	MTF_REGISTER(io_source);
	MTF_REGISTER(h265_dec);
	MTF_REGISTER(io_sink);
	
	char fNameIn[256];
	char fNameOut[256];
	sprintf(fNameIn, "%s%s", PATH, FILE_NAME);

#if 1
	//enc and dec
	sprintf(fNameOut, "%s.yuv", fNameIn);
	void* param[] = {
	(void*)(fNameIn),
	(void*)(fNameOut),
	(void*)(4*1024),
	(void*)(4*1024),
	};
	const char* str = {
	"|io_source,url=$0,oPortMax=$2|-->"
	"|h265_dec|-->"
	"|io_sink,url=$1,iPortMax=$3|"
	};
#endif
	MultiemdiaApi(str, param);
}
#endif