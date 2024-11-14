#if 1
#include"testInner.h"

//int fac_1d160Q10 = 1024/160=6.4
//int fac_1d160Q10 = (1 << 10)/160
int fac_1d160Q10 = 6;

void ttest() {

	for (int x = 0; x < 1024; x++) {
		int y = (x * fac_1d160Q10) >> 10;
		printf("%d / 160 = %d\n", x , y);
		if (y != (int)(x / 160))
			int a = 1;
	}
	//printf("%d\n", sizeof(float));
}


bool testDemo()
{


	int a = 1;

	LOG();
	ttest();
	return 0;
}

#endif