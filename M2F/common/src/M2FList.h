#ifndef __M2F_LIST__H_
#define __M2F_LIST__H_

#include"./M2FObject.h"

typedef struct List
{
	void* p;
	m2f_uint32 size;
	List* next;
};

typedef struct Lists
{
	List* first;
	List* end;
	m2f_uint32 num;
};

List* ListSearch(Lists* lists, void* p);
List* ListSearch(Lists* lists, m2f_uint32 size);


m2f_int32 ListAdd(Lists *lists, List *list);
m2f_int32 ListDelete(Lists* lists, List* list);


#endif