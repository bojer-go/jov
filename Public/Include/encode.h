#pragma once

int GB2312ToUtf8(const char* gb2312, char* utf8);		//GB2312��UTF-8��ת��

int Utf8ToGB2312(const char* utf8, char* gb2312);		//UTF-8��GB2312��ת��

int GB2312ToUnicode(const char* gb2312, char* unicode);	//GB2312��Unicode��ת��

int UnicodeToGB2312(const char* unicode, int size, char*gb2312);//Unicode��GB2312��ת��

int UnicodeToGBK(const wchar_t* unicode, char* gbk);	//Unicodeת����gbk

int GBKToUnicode(const char* gbk, wchar_t* unicode);	//gbkת��unicode

int Utf8ToUnicode(const char* utf8, wchar_t*unicode);	//UTF-8��Unicode��ת��

int UnicodeToUtf8(const wchar_t* unicode, char* utf8);	///Unicode��UTF-8��ת��

int formatJson(char* json, char *des);

char* getLevelStr(int level, char *src);

int md5(char *des, char *src);	//md5

int HexToByte(char *hexStr);