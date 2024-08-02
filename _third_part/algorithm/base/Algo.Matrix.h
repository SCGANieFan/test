#pragma once

#include"Algo.Type.h"
#include"Algo.Macro.h"
// #include"Algo.Memory.h"
// #include"Algo.Buffer.h"

namespace Algo {

template<class T>
struct Matrix_t {
	T* data;
	int m;
	int n;
};

template<class T,int M,int N>
struct MatrixProduct_t;

template<class T>
struct MatrixProduct_t<T, -1, -1> {
	static inline void Run(Matrix_t<T>* A, Matrix_t<T>* B, Matrix_t<T>* C, void (*PostDeal)(T*)) {
		if ((A->n != B->m)
			||A->m!=C->m
			||B->n!=C->n)
			return;
		T* av = A->data;
		T* bv = B->data;
		T* cv = C->data;
		memset(cv, 0, sizeof(T) * C->m * C->n);
		for (int am = 0; am < A->m; am++) {
			bv = B->data;
			for (int an = 0; an < A->n; an++) {
				for (int bn = 0; bn < B->n; bn++) {
					cv[bn] += av[0] * bv[bn];
				}
				av++;
				bv+= B->n;
			}
			for (int cn = 0; cn < C->n; cn++) {
				PostDeal(&cv[cn]);
			}
			cv += C->n;
		}
	}
};

template<class T>
struct MatrixProduct_t<T, 4, 4> {
	static inline void Run(Matrix_t<T>* A, Matrix_t<T>* B, Matrix_t<T>* C, void (*PostDeal)(T*)) {
		const int M = 4;
		const int N = 4;
		T* av = A->data;
		T* bv = B->data;
		T* cv = C->data;
		memset(cv, 0, sizeof(T) * M * N);
		for (int am = 0; am < M; am++) {
			bv = B->data;
			for (int an = 0; an < N; an++) {
				T avv = av[0];
				av++;
				cv[0] += avv * bv[0];
				cv[1] += avv * bv[1];
				cv[2] += avv * bv[2];
				cv[3] += avv * bv[3];
				bv += N;
			}
			PostDeal(&cv[0]);
			PostDeal(&cv[1]);
			PostDeal(&cv[2]);
			PostDeal(&cv[3]);
			cv += N;
		}
	}
};

static inline void MatrixTranspositionProduct(int16_t* A, int16_t* B, int16_t* C) {
	const int N = 4;//m*n=n*n
	const int16_t* av = A;
	const int16_t* bv = B;
	int16_t* cv = C;
	memset(cv, 0, sizeof(int16_t) * N * N);
	for (int an = 0; an < N; an++) {
		av = A + an;
		bv = B;
		for (int am = 0; am < N; am++) {
			for (int bm = 0; bm < N; bm++) {
				cv[bm] += av[0] * bv[bm];
			}
			bv+=N;
			av += N;
		}
		cv += N;
	}
}


#if 0
//test demo
	int16_t aData[4][4] = { {0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15} };
	int16_t bData[4][4] = { {1,0,0,0},{0,1,0,0} ,{0,0,1,0} ,{0,0,0,1} };
	int16_t cData[4][4] = {0};
	Matrix_t<int16_t> A{(int16_t*)aData,4,4};
	Matrix_t<int16_t> B{(int16_t*)bData,4,4};
	Matrix_t<int16_t> C{(int16_t*)cData,4,4};
	auto PostDeal = [](int16_t* val) {*val = *val * 2; };
	MatrixProduct_t<int16_t, 4, 4>::Run(&A, &B, &C, PostDeal);
#endif
}

