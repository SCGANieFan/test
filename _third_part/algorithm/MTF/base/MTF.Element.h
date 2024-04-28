#pragma once
#include"MTF.Object.h"
#include"MTF.Data.h"
#if 0
enum MTF_Ret
{
	
};
#endif
class MTF_Element :public MTF_Object
{
public:
	MTF_Element();
	~MTF_Element();
public:
	virtual mtf_int32 Pull(MTF_Data*& iData) = 0;
	virtual mtf_int32 Push(MTF_Data& oData) = 0;

	virtual mtf_int32 Receive(MTF_Data& iData) = 0;
	virtual mtf_int32 Generate(MTF_Data*& oData) = 0;
	virtual mtf_int32 Run() = 0;

	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) override;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) override;

private:

};

