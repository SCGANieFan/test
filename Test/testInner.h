#pragma once
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdarg.h>
#include<iostream>
#include<string>
#include<vector>
#include<assert.h>

#define LOG(fmt,...) printf("<%s>(%s)[%d]" fmt "\n", __FILE__,__func__,__LINE__,##__VA_ARGS__)



//
bool testDemo();
//bool CodecTest();

//BitPerSampleConvert
bool TestBitPerSampleConvet();

//StackTest
bool StackTest();

//FrameTest()
bool FrameTest();
//bool OggTest();

void FuncRun();

bool testThread();

void testFFT();