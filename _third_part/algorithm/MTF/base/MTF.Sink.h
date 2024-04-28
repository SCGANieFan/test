#pragma once
#include"MTF.Element.h"
class MTF_Sink :public MTF_Element
{
public:
	MTF_Sink();
	~MTF_Sink();
private:
	virtual mtf_int32 Pull(MTF_Data*& iData) final;
	virtual mtf_int32 Push(MTF_Data& oData)final { return 0; };
	virtual mtf_int32 Receive(MTF_Data& iData) override;
	virtual mtf_int32 Generate(MTF_Data*& oData)final { return 0; };
	virtual mtf_int32 Run() final;	
public:
	virtual mtf_int32 receive(MTF_Data& iData) = 0;
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) override;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) override;
protected:
private:
	MTF_Element* _from;

};

