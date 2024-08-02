#pragma once
#include"MTF.Object.h"
#include"MTF.Memory.h"
#include<new>
class MTF_Objects
{
protected:
	typedef struct
	{
		mtf_void* create;
		const mtf_int8* type;
	}Item;
public:
	MTF_Objects();
	~MTF_Objects();
public:
	template<class T>
	static bool Registe(const mtf_int8* type)
	{
		for (auto& item : objects._items)
		{
			if (!item.create)
			{
				item.create = []() {
					mtf_void* buff = MTF_MALLOC(sizeof(T));
					return new(buff) T();
				};
				item.type = type;
				return true;
			}
			if (IsTypeEqual(item.type, type)) {
				item.create = []() {
					mtf_void* buff = MTF_MALLOC(sizeof(T));
					return new(buff) T();
				};
				return true;
			}
		}
		return false;
	}
	static mtf_void* Create(const mtf_int8* type);

private:
	static Item* Search(const mtf_int8* type);
	static bool IsTypeEqual(const mtf_int8* type0, const mtf_int8* type1);
private:
	static MTF_Objects objects;
	Item _items[10];
};

