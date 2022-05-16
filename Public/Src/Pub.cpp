#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "Pub.h"

char * S2C(CString str)
{
	if(str.IsEmpty()) return "";
#ifdef UNICODE
	wchar_t *temp = (LPTSTR)(LPCTSTR)str;//��õ���CString�ĵ�ַ�������޸�ָ���ֵ
	return W2C(temp);
#else
	return (LPTSTR)(LPCTSTR)str;		//����ת����char*
#endif
}

//�Լ�����ռ䣬�Ƽ�ʹ��
char * S2C(char *des, CString src)
{
	if(src.IsEmpty()) 
	{
		des[0] = '\0';
		return "";
	}
#ifdef UNICODE
	wchar_t *temp = (LPTSTR)(LPCTSTR)src;//��õ���CString�ĵ�ַ�������޸�ָ���ֵ
	return W2C(des, temp);
#else
	strcpy(des, (LPTSTR)(LPCTSTR)src);
	return (LPTSTR)(LPCTSTR)src;		//����ת����char*
#endif
}

//�̲߳���ȫ
char * W2C(wchar_t *str)
{
	/*��Ϊ���ص��ڴ�Ҫ��ͣ���������ͷţ������ͷźܲ����㣬
	����þֲ���̬�����������ڳ�������Ϊ10240�ֽڣ�ʹ��ʱ
	ҪС�ģ������ȴ���10240ʱҪ����չ*/
	static char p[10240];//10k
	memset(p, 0, sizeof(char)*10240);//p�ᱣ���ϴεĽ���������ó�ʼ��һ��
	int len= WideCharToMultiByte(CP_ACP,0,str,wcslen(str),NULL,0,NULL,NULL);   
    if (len >= 10240) 
		return NULL;
	WideCharToMultiByte(CP_ACP,0,str,wcslen(str),p,len,NULL,NULL);  
    p[len]='\0';  
	return p;
}

//�Ƽ�ʹ��������������ݵĿռ�����ɵ��õ��Լ�����
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
		if(nPos>=0){//���ҵ�
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
		temp = temp.Mid(nPos + 1);//���м�ȡֵ
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