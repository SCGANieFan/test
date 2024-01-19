#include"./M2FList.h"

List* ListSearch(Lists* lists, void* p)
{
	List* list = lists->first;
	for (m2f_uint32 i = 0; i < lists->num; i++)
	{
		if (list->p == p)
		{
			return list;
		}
		list = list->next;
	}
	return NULL;
}
List* ListSearch(Lists* lists, m2f_uint32 size)
{
	List* list = lists->first;
	for (m2f_uint32 i = 0; i < lists->num; i++)
	{
		if (list->size > size)
		{
			return list;
		}
		list = list->next;
	}
	return NULL;
}
m2f_int32 ListAdd(Lists* lists, List* list)
{
	if (lists->end == NULL)
	{
		lists->first = list;
		lists->end = list;
		list->next = NULL;
	}
	else
	{
		lists->end->next = list;
		list->next = NULL;		
		lists->end = list;
	}
	lists->num += 1;
	return 0;
}

m2f_int32 ListDelete(Lists* lists, List* list)
{
	List* listFront = NULL;
	List* listSearch = lists->first;
	for (m2f_uint32 i = 0; i < lists->num; i++)
	{
		if (listSearch->p == list->p)
		{
			break;
		}
		listFront = listSearch;
		listSearch = listSearch->next;
	}

	if (listFront == lists->end)
	{
		return -1;
	}
	if (listFront == NULL)
	{
		lists->first = list->next;
	}
	else if(listSearch == lists->end)
	{
		listFront->next = NULL;
		lists->end = listFront;
	}
	else
	{
		listFront->next = list->next;
	}
	lists->num -= 1;

	return 0;
}


