#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <string>

#include "Pub.h"
#include <windows.h>

#include "md5.h"

using namespace std;
//GB2312到UTF-8的转换
int GB2312ToUtf8(const char* gb2312, char* utf8)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return len;
}

//UTF-8到GB2312的转换
int Utf8ToGB2312(const char* utf8, char* gb2312)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, gb2312, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return len;
}

//GB2312到Unicode的转换
int GB2312ToUnicode(const char* gb2312, char* unicode)
{
	UINT nCodePage = 936; //GB2312
	int len = MultiByteToWideChar(nCodePage, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(nCodePage, 0, gb2312, -1, wstr, len);
	len = len*sizeof(wchar_t);
	memcpy(unicode, wstr, len);
	if(wstr) delete[] wstr;
	return len;
}


//Unicode到GB2312的转换
int UnicodeToGB2312(const char* unicode, int size, char*gb2312)
{
	UINT nCodePage = 936; //GB2312
	wchar_t* wstr = new wchar_t[size/2+1];
	memcpy(wstr, unicode, size);
	int len = WideCharToMultiByte(nCodePage, 0, wstr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(nCodePage, 0, wstr, -1, gb2312, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return len;
}

//Unicode转换到gbk
int UnicodeToGBK(const wchar_t* unicode, char* gbk)
{
	int len = WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, unicode, -1, szUtf8, len, NULL, NULL);
	memcpy(gbk, szUtf8, len);
	if (szUtf8) delete[] szUtf8;

	return len;
}

//gbk转到unicode
int GBKToUnicode(const char* gbk, wchar_t* unicode)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gbk, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gbk, -1, wstr, len);
	wmemcpy(unicode, wstr, len);
	if (wstr) delete[] wstr;
	return len;
}

//UTF-8到Unicode的转换
int Utf8ToUnicode(const char* utf8, wchar_t*unicode){
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	wmemcpy(unicode, wstr, len);
	if(wstr) delete[] wstr;
	return len;
}

//Unicode到UTF-8的转换
int UnicodeToUtf8(const wchar_t* unicode, char* utf8)
{
	int len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
    char *szUtf8 = (char*)malloc(len + 1);
    memset(szUtf8, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	memcpy(utf8, szUtf8, len);
	if(szUtf8) delete[] szUtf8;

	return len;
}

int formatJson(char* json, char *des)
{
	if (des == NULL || json == NULL)
		return -1;
	char *result = des;

	int level = 0;
	int length = strlen(json);
	for (int index = 0; index < length; index++)
	{
		char c = json[index];

		if (level > 0 && '\n' == json[length - 1])
		{
			getLevelStr(level, result);
		}

		switch (c)
		{
		case '{':
		case '[':
			result[strlen(result)] = c;
			strcat(result, "\r\n");
			level++;
			getLevelStr(level, result);
			break;
		case ',':
			result[strlen(result)] = c;
			strcat(result, "\r\n");
			getLevelStr(level, result);
			break;
		case '}':
		case ']':
			strcat(result, "\r\n");
			level--;
			getLevelStr(level, result);
			result[strlen(result)] = c;
			break;
		default:
			result[strlen(result)] = c;
			break;
		}

	}
	return strlen(result);
}

char* getLevelStr(int level, char *src)
{
	char *p = src;
	for (int i = 0; i < level; i++)
	{
		strcat(p, "\t");
	}
	return src;
}

int md5(char *des, char *src)
{
	MD5_CTX mdContext;
	MD5Init(&mdContext);
	int len = strlen(src);
	string strPlain = src;
	MD5Update(&mdContext, (unsigned char*)const_cast<char*>(strPlain.c_str()), strPlain.size());
	MD5Final(&mdContext);
	char buf[3];
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(buf, "%02x", mdContext.digest[i]);
		strcat(des, buf);
	}


	return 1;
}

int HexToByte(char *hexStr)
{
	char result;
	if (hexStr[0] >= 'a' && hexStr[0] <= 'f')
		result = hexStr[0] - 'a' + 10;
	else if (hexStr[0] >= 'A' && hexStr[0] <= 'F')
		result = hexStr[0] - 'A' + 10;
	else
		result = hexStr[0] - '0';

	if (hexStr[1] >= 'a' && hexStr[1] <= 'f')
		result = (result << 4) | (hexStr[1] - 'a' + 10);
	else if (hexStr[1] >= 'A' && hexStr[1] <= 'F')
		result = (result << 4) | (hexStr[1] - 'A' + 10);
	else 
		result = (result << 4) | (hexStr[1] - '0');

	return result;
}