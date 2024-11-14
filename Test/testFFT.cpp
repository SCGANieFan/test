#if 1
#include"testInner.h"

#if 0
typedef struct
{
	double real;
	double imag;
}Complex;

/* 复数加法 */
void Complex_ADD(Complex a, Complex b, Complex* c)
{
	c->real = a.real + b.real;
	c->imag = a.imag + b.imag;
}
/* 复数减法 */
void Complex_SUB(Complex a, Complex b, Complex* c)
{
	c->real = a.real - b.real;
	c->imag = a.imag - b.imag;
}
/* 复数乘法 */
void Complex_MUL(Complex a, Complex b, Complex* c)
{
	c->real = a.real * b.real - a.imag * b.imag;
	c->imag = a.real * b.imag + a.imag * b.real;
}
/* 复数绝对值 */
double Complex_ABS(Complex* a)
{
	double b;
	b = sqrt((a->real) * (a->real) + (a->imag) * (a->imag));
	return b;
}

static double PI = 4.0 * atan(1); //定义π 因为tan(π/4)=1 所以arctan（1）*4=π，增加π的精度


uint32_t FFT(uint32_t* signal_in, uint32_t signal_len, Complex* fft_out, uint32_t fft_point)
{
	LOG();
	Complex* W;//旋转因子Wn
	uint32_t i, j, k, m;
	Complex temp1, temp2, temp3;//用于交换中间量
	double series;//序列级数
#if 1
	if (signal_len < fft_point)//采样点数与信号数据对比 
	{
		for (i = 0; i < signal_len; i++)
		{
			fft_out[i].real = signal_in[i];
			fft_out[i].imag = 0;
		}
		for (i = signal_len; i < fft_point; i++)//补0
		{
			fft_out[i].real = 0;
			fft_out[i].imag = 0;
		}
	}
	else if (signal_len == fft_point)
	{
		for (i = 0; i < signal_len; i++)
		{
			fft_out[i].real = signal_in[i];
			fft_out[i].imag = 0;
		}
	}
	else
	{
		for (i = 0; i < fft_point; i++)
		{
			fft_out[i].real = signal_in[i];
			fft_out[i].imag = 0;
		}
	}
#endif

	if (fft_point % 2 != 0)
	{
		return 1;
	}

	W = (Complex*)malloc(sizeof(Complex) * fft_point);
	if (W == NULL)
	{
		return 1;
	}

	for (i = 0; i < fft_point; i++)
	{
		W[i].real = cos(2 * PI / fft_point * i);	 //欧拉表示的实部
		W[i].imag = -1 * sin(2 * PI / fft_point * i);//欧拉表示的虚部
	}
#if 1
	//重排序
	for (i = 0; i < fft_point; i++)
	{
		k = i;
		j = 0;
		series = log(fft_point) / log(2); //算出序列的级数 log2(fft_point)
		while ((series--) > 0)//利用按位与以及循环实现码位颠倒
		{
			j = j << 1;
			j |= (k & 1);
			k = k >> 1;
		}
		if (j > i) //将x(n)的码位互换
		{
			temp1 = fft_out[i];
			fft_out[i] = fft_out[j];
			fft_out[j] = temp1;
		}
	}
#endif
	series = log(fft_point) / log(2); //蝶形运算的级数

	for (i = 0; i < series; i++)
	{
		m = 1 << i; //移位 每次都是2的指数的形式增加，其实也可以用m=2^i代替
		for (j = 0; j < fft_point; j += 2 * m) //一组蝶形运算，每一组的蝶形因子乘数不同
		{
			for (k = 0; k < m; k++)//蝶形结点的距离  一个蝶形运算 每个组内的蝶形运算
			{
				Complex_MUL(fft_out[k + j + m], W[fft_point * k / 2 / m], &temp1);
				Complex_ADD(fft_out[j + k], temp1, &temp2);
				Complex_SUB(fft_out[j + k], temp1, &temp3);
				fft_out[j + k] = temp2;
				fft_out[j + k + m] = temp3;
			}
		}
	}
	free(W);
	return 0;
}

#endif

#if 1
#include <stdio.h>  
#include <stdlib.h>  
#include <math.h>  

// 定义PI  
#ifndef PI  
#define PI 3.14159265358979323846  
#endif  

// 定义复数结构体  
typedef struct {
	double real;
	double imag;
} complex_t;

// 复数相加  
complex_t complex_add(complex_t a, complex_t b) {
	complex_t result;
	result.real = a.real + b.real;
	result.imag = a.imag + b.imag;
	return result;
}

// 复数相减  
complex_t complex_sub(complex_t a, complex_t b) {
	complex_t result;
	result.real = a.real - b.real;
	result.imag = a.imag - b.imag;
	return result;
}

// 复数相乘（包括复数与实数的乘法）  
complex_t complex_mul(complex_t a, complex_t b) {
	complex_t result;
	result.real = a.real * b.real - a.imag * b.imag;
	result.imag = a.real * b.imag + a.imag * b.real;
	return result;
}

// 复数乘以实数  
complex_t complex_mul_real(complex_t a, double r) {
	complex_t result;
	result.real = a.real * r;
	result.imag = a.imag * r;
	return result;
}

// 交换两个复数  
void swap(complex_t* a, complex_t* b) {
	complex_t temp = *a;
	*a = *b;
	*b = temp;
}

// 计算FFT  
void fft(complex_t* X, int n) {
	if (n <= 1) return;

	// 分配内存给偶数和奇数索引的元素  
	complex_t* X_even = (complex_t*)malloc(n / 2 * sizeof(complex_t));
	complex_t* X_odd = (complex_t*)malloc(n / 2 * sizeof(complex_t));
	for (int i = 0; i < n / 2; i++) {
		X_even[i] = X[i * 2];
		X_odd[i] = X[i * 2 + 1];
	}

	// 递归计算FFT  
	fft(X_even, n / 2);
	fft(X_odd, n / 2);

	// 预处理旋转因子  
	for (int k = 0; k < n / 2; k++) {
		double angle = -2 * PI * k / n;
		complex_t w = { cos(angle), sin(angle) };
		complex_t t = complex_mul(w, X_odd[k]);

		// 组合结果  
		X[k] = complex_add(X_even[k], t);
		X[k + n / 2] = complex_sub(X_even[k], t);
	}

	// 释放内存  
	free(X_even);
	free(X_odd);
}

// 打印复数数组  
void print_complex_array(complex_t* X, int n) {
	for (int i = 0; i < n; i++) {
		printf("%.5f + %.5fi\n", X[i].real, X[i].imag);
	}
}


#endif

void testFFT()
{
	LOG();
#if 0
	const uint32_t signal_len = 4;
	uint32_t signal_in[signal_len];
	signal_in[0] = 0;
	signal_in[1] = 1;
	signal_in[2] = 0;
	signal_in[3] = 1;
	const uint32_t fft_point = signal_len;
	Complex fft_out[fft_point];
	FFT(signal_in, signal_len, fft_out, fft_point);
#endif

#if 1
	// 示例数据，一个包含8个复数的数组  
	complex_t X[] = {
		{0.0, 0.0}, 
		{1.0, 0.0},
		{0.0, 0.0}, 
		{1.0, 0.0},
		{1.0, 0.0}
	};

	int n = sizeof(X) / sizeof(X[0]);

	// 打印原始数据  
	printf("Original data:\n");
	print_complex_array(X, n);

	// 计算FFT  
	fft(X, n);

	// 打印FFT结果  
	printf("\nFFT result:\n");
	print_complex_array(X, n);
#endif
	while (1);
}

#endif