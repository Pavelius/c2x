#include "collection.h"

void collection::sort(int i1, int i2, int(*compare)(const void* p1, const void* p2, void* param), void* param)
{
	for(int i = i2; i > i1; i--)
	{
		for(int j = i1; j < i; j++)
		{
			auto t1 = get(j);
			auto t2 = get(j + 1);
			if(compare(t1, t2, param) > 0)
				swap(j, j + 1);
		}
	}
}