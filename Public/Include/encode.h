#pragma once

int GB2312ToUtf8(const char* gb2312, char* utf8);		//GB2312到UTF-8的转换

int Utf8ToGB2312(const char* utf8, char* gb2312);		//UTF-8到GB2312的转换

int GB2312ToUnicode(const char* gb2312, char* unicode);	//GB2312到Unicode的转换

int UnicodeToGB2312(const char* unicode, int size, char*gb2312);//Unicode到GB2312的转换

int UnicodeToGBK(const wchar_t* unicode, char* gbk);	//Unicode转换到gbk

int GBKToUnicode(const char* gbk, wchar_t* unicode);	//gbk转到unicode

int Utf8ToUnicode(const char* utf8, wchar_t*unicode);	//UTF-8到Unicode的转换

int UnicodeToUtf8(const wchar_t* unicode, char* utf8);	///Unicode到UTF-8的转换

int formatJson(char* json, char *des);

char* getLevelStr(int level, char *src);

int md5(char *des, char *src);	//md5

int HexToByte(char *hexStr);