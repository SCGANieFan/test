#pragma once
#include"MTF.Element.h"
class MTF_Source :public MTF_Element
{
public:
	MTF_Source();
	~MTF_Source();
private:
	virtual mtf_int32 Pull(MTF_Data*& iData)final { return 0; };
	virtual mtf_int32 Push(MTF_Data& oData) final;
	virtual mtf_int32 Receive(MTF_Data& iData)final { return 0; };
	virtual mtf_int32 Generate(MTF_Data*& oData) override;
	virtual mtf_int32 Run() final;
public:
	virtual mtf_int32 generate(MTF_Data*& oData) = 0;
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) override;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) override;

protected:
private:
	MTF_Element* _to;
};

