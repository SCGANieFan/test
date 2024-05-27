#if 1
//#include<string.h>
//#include<stdint.h>
#include<stdio.h>
//#include<stdarg.h>
#include<iostream>
#include<string>
//#include<vector>


template<int shiftType, class Ti, class To, class Tx>
class Algo_Shift {};

template<class Ti, class To, class Tx>
class Algo_Shift< 1, Ti, To, Tx > {
public:
	inline To DoShift(Ti x, int8_t shiftNum) {
		return (To)(((Tx)x) >> shiftNum);
	}
};

template<class Ti, class To, class Tx>
class Algo_Shift< -1, Ti, To, Tx > {
public:
	inline To DoShift(Ti x, int8_t shiftNum) {
		return (To)(((Tx)x) << shiftNum);
	}
};

template<class Ti, class To, class Tx>
class Algo_Shift< 0, Ti, To, Tx > {
public:
	inline To DoShift(Ti x, int8_t shiftNum) {
		return (To)(x);
	}
};



bool testDemo()
{

	int x = 3;

	Algo_Shift<-1,int, int, int> lShift;
	printf("(%d << 1) = %d\n", x, lShift.DoShift(x, 1));
	Algo_Shift<1,int, int, int> rShift;
	printf("(%d >> 1) = %d\n", x, rShift.DoShift(x, 1));
	return 0;
}


#endif