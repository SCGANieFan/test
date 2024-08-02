#pragma once
#include<new>
#include"MAF.Type.h"

class MAF_Object
{
protected:
    typedef struct
    {
        maf_void* create;
        maf_void* getSize;
        const maf_int8 *type;
    }Item;
public:
	MAF_Object();
	~MAF_Object();

public:
	template<class T>
    static maf_bool Registe(const maf_int8* type)
	{
		for (auto& item : object._items)
		{
			if (IsTypeEqual(item.type, type)) {
				item.create = [](maf_void* buff) {return new(buff) T(); };
				item.getSize = []() {return sizeof(T); };
				return true;
			}

			if (!item.create)
			{
				item.create = [](maf_void* buff) {return new(buff) T(); };
				item.type = type;
				item.getSize = []() {return sizeof(T); };
				return true;
			}
		}
		return false;
	}

    static maf_int32 GetSize(const maf_int8* type);
    static maf_void* Create(const maf_int8* type, maf_void* buff);


private:
	static Item *Search(const maf_int8* type);
	static maf_bool IsTypeEqual(const maf_int8* type0, const maf_int8* type1);
private:
	static MAF_Object object;
	Item _items[10];
};

