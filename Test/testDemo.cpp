#include"testInner.h"


void func()
{
	int i=0;
	int j = 1;
	int k = 1;
	int e = 1;
	while (1)
	{
		switch (i)
		{
		case 0:
			j = 1;
			break;
		case 1:
			if(k == 1)
				break;
			break;
		case 2:
			e = 1;
			break;
		default:
			break;
		}
		i++;
	}

	printf("i:%d\n", i);
}



bool testDemo()
{
	func();
	return 1;
}