#if 1
//#include<string.h>
//#include<stdint.h>
#include<stdio.h>
//#include<stdarg.h>
#include<iostream>
#include<string>
//#include<vector>



#define PATH "./"
#define FILE_NAME "text.txt"

bool testDemo()
{
	char x[4] = { 1,2,3,4 };
	FILE* fp;
	fp = fopen(PATH FILE_NAME, "wb");
	if (!fp)
		return false;
	const char* data = "0,1,2";
	const char* head = "head";
	fwrite(data, 1, 5, fp);
	fseek(fp, 0, SEEK_SET);
	//fwrite(head, 1, 4, fp);
	fprintf(fp, "%s", head);
	fclose(fp);
	return 0;
}


#endif