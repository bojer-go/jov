#pragma once

#define LENGTH_1M 1024*1024
#define LENGTH_1K 1024

char * S2C(CString str);	//CStringת��Ϊchar 
char * S2C(char *des, CString src);
char * W2C(wchar_t *str);	//���ֽڵ�charת��Ϊchar
char * W2C(char *des, wchar_t *src);

double RoundEx(const double &dIn);	//ȡ��
double RoundEx(double dVal, short iPlaces);

int GetStr(CString &src, CString &des, int pos, char Separator='|');//��ȡ�ָ����еĵڼ�����

char *GetRootPathEx(char *path);