#if 1
#include"testInner.h"

#if 1
template<class Ti, class To>
class DataConvert_c {
public:
	DataConvert_c() {}
	~DataConvert_c() {}
public:
	static void Run(void* in, int strideIn, void* out, int strideOut,int sample) {
		Ti* pIn = (Ti*)in;
		To* pOut = (To*)out;
		for (int s = 0; s < sample; s++) {
			pOut[0] = pIn[0];
			pIn += strideIn;
			pOut += strideOut;
		}
	}
private:

};
#endif


#if 1
template<class T1, class T2>
struct IsTypeSame_t
{
	operator bool() { return false; }
};

template<class T>
struct IsTypeSame_t<T,T>
{
	operator bool() { return true; }
};
#endif

#if 1
template<class Ti, class Tm, class To>
class AudioSampleDeal_c
{
public:
	typedef int (*RUN_CB)(AudioSampleDeal_c*, void*);
public:
	AudioSampleDeal_c() {}
	~AudioSampleDeal_c() {}
public:
	void init() {
		bool isConvertInput = !IsTypeSame_t<Ti, Tm>();
		bool isConvertOutput = !IsTypeSame_t<Tm, To>();
		if (isConvertInput && isConvertInput) {
			run_cb = RunConvertInOut_s;
		}
		else if (isConvertInput) {
			run_cb = RunConvertIn_s;
		}
		else if (isConvertOutput) {
			run_cb = RunConvertOut_s;
		}
		else {
			run_cb = RunNoConvert_s;
		}

	}
public:
	int Run(void* usr) {
		return run_cb(this, usr);
	}
private:
	virtual int RunConvertInOut(void* usr) = 0;
	virtual int RunConvertIn(void* usr) = 0;
	virtual int RunConvertOut(void* usr) = 0;
	virtual int RunNoConvert(void* usr) = 0;
private:
	static int RunConvertInOut_s(AudioSampleDeal_c* hd, void* usr) { return hd->RunConvertInOut(usr); }
	static int RunConvertIn_s(AudioSampleDeal_c* hd, void* usr) { return hd->RunConvertIn(usr); }
	static int RunConvertOut_s(AudioSampleDeal_c* hd, void* usr) { return hd->RunConvertOut(usr); }
	static int RunNoConvert_s(AudioSampleDeal_c* hd, void* usr) { return hd->RunNoConvert(usr); }
private:
	RUN_CB run_cb = 0;
};


#endif


#if 1
#if 1
template<class T>
static inline int RunCommon(T* in0, T* in1, T* out, int sample) {
	for (int s = 0; s < sample; s++) {
		out[s] = in0[s] + in1[s];
	}
	return 0;
}

template<>
static inline int RunCommon<float>(float* in0, float* in1, float* out, int sample) {
	for (int s = 0; s < sample; s++) {
		out[s] = in0[s] + in1[s];
	}
	return 0;
}
#endif


template<class Ti, class Tm, class To>
class ADD_c:public AudioSampleDeal_c<Ti, Tm, To>
{
public:
	class Param_c {
	public:
		Param_c() {}
		~Param_c() {}
	public:
		void* in0;
		void* in1;
		void* out;
		int samples;
	};
public:
	ADD_c() {}
	~ADD_c() {}
public:
	void init() {
		AudioSampleDeal_c<Ti, Tm, To>::init();
	}
private:
	virtual int RunConvertInOut(void* usr)final;
	virtual int RunConvertIn(void* usr)final;
	virtual int RunConvertOut(void* usr)final;
	virtual int RunNoConvert(void* usr)final;
};

template<class Ti, class Tm, class To>
int ADD_c<Ti, Tm, To>::RunConvertInOut(void* usr) {
	Param_c* param = (Param_c*)usr;
	Tm inTmp0[100];
	Tm inTmp1[100];
	Tm outTmp[100];
	DataConvert_c<Ti, Tm>::Run(param->in0, 1, inTmp0, 1, param->samples);
	DataConvert_c<Ti, Tm>::Run(param->in1, 1, inTmp1, 1, param->samples);
	int ret = RunCommon((Tm*)inTmp0, (Tm*)inTmp1, (Tm*)outTmp,param->samples);
	DataConvert_c<Tm, To>::Run(outTmp, 1, param->out, 1, param->samples);
	return ret;
}
template<class Ti, class Tm, class To>
int ADD_c<Ti, Tm, To>::RunConvertIn(void* usr) {
	Param_c* param = (Param_c*)usr;
	Tm inTmp0[100];
	Tm inTmp1[100];
	DataConvert_c<Ti, Tm>::Run(param->in0, 1, inTmp0, 1, param->samples);
	DataConvert_c<Ti, Tm>::Run(param->in1, 1, inTmp1, 1, param->samples);
	return RunCommon((Tm*)inTmp0, (Tm*)inTmp1, (Tm*)param->out,param->samples);
}
template<class Ti, class Tm, class To>
int ADD_c<Ti, Tm, To>::RunConvertOut(void* usr) {
	Param_c* param = (Param_c*)usr;
	Tm outTmp[100];
	int ret = RunCommon((Tm*)param->in0, (Tm*)param->in1, (Tm*)outTmp,param->samples);
	DataConvert_c<Tm, To>::Run(outTmp, 1, param->out, 1, param->samples);
	return ret;
}
template<class Ti, class Tm, class To>
int ADD_c<Ti, Tm, To>::RunNoConvert(void* usr) {
	Param_c* param = (Param_c*)usr;
	return RunCommon((Tm*)param->in0, (Tm*)param->in1, (Tm*)param->out,param->samples);
}

#endif

bool testDataTypeConvert()
{
	LOG();
#if 1
	const int sample = 10;
	int x0[sample];
	int x1[sample];
	int y[sample];
	for (int s = 0; s < sample; s++) {
		x0[s] = s;
		x1[s] = s+2;
	}
	ADD_c<int, float, int> add;
	add.init();
	add.init();
	ADD_c<int, float, int>::Param_c para;
	para.in0 = x0;
	para.in1 = x1;
	para.out = y;
	para.samples = sample;
	add.Run(&para);
	for (int s = 0; s < sample; s++) {
		LOG("%d", y[s]);
	}
	return 0;
#endif
	return 0;
}

#endif