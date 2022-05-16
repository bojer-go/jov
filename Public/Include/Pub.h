#pragma once

#define LENGTH_1M 1024*1024
#define LENGTH_1K 1024

char * S2C(CString str);	//CString转换为char 
char * S2C(char *des, CString src);
char * W2C(wchar_t *str);	//宽字节的char转换为char
char * W2C(char *des, wchar_t *src);

double RoundEx(const double &dIn);	//取整
double RoundEx(double dVal, short iPlaces);

int GetStr(CString &src, CString &des, int pos, char Separator='|');//获取分隔符中的第几个数

char *GetRootPathEx(char *path);