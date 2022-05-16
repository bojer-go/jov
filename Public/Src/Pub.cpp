#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Pub.h"

char * S2C(CString str)
{
	if(str.IsEmpty()) return "";
#ifdef UNICODE
	wchar_t *temp = (LPTSTR)(LPCTSTR)str;//获得的是CString的地址，不能修改指向的值
	return W2C(temp);
#else
	return (LPTSTR)(LPCTSTR)str;		//可以转换成char*
#endif
}

//自己传入空间，推荐使用
char * S2C(char *des, CString src)
{
	if(src.IsEmpty()) 
	{
		des[0] = '\0';
		return "";
	}
#ifdef UNICODE
	wchar_t *temp = (LPTSTR)(LPCTSTR)src;//获得的是CString的地址，不能修改指向的值
	return W2C(des, temp);
#else
	strcpy(des, (LPTSTR)(LPCTSTR)src);
	return (LPTSTR)(LPCTSTR)src;		//可以转换成char*
#endif
}

//线程不安全
char * W2C(wchar_t *str)
{
	/*因为返回的内存要不停的申请与释放，而且释放很不方便，
	这儿用局部静态变量，但由于长度限制为10240字节，使用时
	要小心，当长度大于10240时要做扩展*/
	static char p[10240];//10k
	memset(p, 0, sizeof(char)*10240);//p会保留上次的结果，这儿最好初始化一下
	int len= WideCharToMultiByte(CP_ACP,0,str,wcslen(str),NULL,0,NULL,NULL);   
    if (len >= 10240) 
		return NULL;
	WideCharToMultiByte(CP_ACP,0,str,wcslen(str),p,len,NULL,NULL);  
    p[len]='\0';  
	return p;
}

//推荐使用这个方法，数据的空间分配由调用的自己分配
char * W2C(char *des, wchar_t *src)
{
	int len= WideCharToMultiByte(CP_ACP,0,src,wcslen(src),NULL,0,NULL,NULL);   
	WideCharToMultiByte(CP_ACP,0,src,wcslen(src),des,len,NULL,NULL);  
    des[len]='\0';  
	return des;
}

double RoundEx(const double &dIn)
{
	double dLast = dIn;
    if (dIn >= 0.0) 
        return int(dLast + 0.5);
    else
        return int(dLast - 0.5);
}

//double RoundEx(double dVal, short iPlaces) 
//{
//    double dRetval = 0;
//	double dMol = 5.0/pow(10.0, iPlaces+1);
//	double dPlace = pow(10.0, iPlaces);
//    if (dVal >= 0.0)
//	{
//		int temp = (int)((dVal + dMol)*dPlace);
//		dRetval = (float)(temp/dPlace);
//	}
//	else
//	{
//		int temp = (int)((dVal - dMol)*dPlace);
//		dRetval = (float)(temp/dPlace);
//	}
//
//    return(dRetval);
//}
 
double RoundEx(double dVal, short iPlaces)
{
    char s[20];
    double dRetval;
 
    sprintf(s,"%.*lf",iPlaces, dVal);
	dRetval = atof(s);
    return (dRetval);
}

int GetStr(CString &src, CString &des, int pos, char Separator)
{
	int len = 0;
	int count = 0;
	CString temp = src;
	while(!temp.IsEmpty())
	{
		int nPos = temp.Find(Separator);
		count++;
		if(nPos>=0){//若找到
			if(count == pos)
			{
				des = temp.Left(nPos);
				len = des.GetLength();
				return len;
			}
		}
		else if(count == pos)
		{
			des = temp;
			len = des.GetLength();
			return len;
		}
		temp = temp.Mid(nPos + 1);//从中间取值
		temp.TrimLeft();
	}

	return len;
}

char *GetRootPathEx(char *path)
{
	CString sFileName;

	GetModuleFileName(NULL, sFileName.GetBufferSetLength(MAX_PATH+1), MAX_PATH);
	sFileName.ReleaseBuffer();
	int nPos = sFileName.ReverseFind('\\');
	sFileName = sFileName.Left(nPos); 

	S2C(path, sFileName);

	return path;
}