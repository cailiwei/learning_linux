/*文件名:exp4-4.cpp*/
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstdio>
using namespace std;

#define MaxSize 100
typedef struct
{
	char ch[MaxSize];
	int len;			/*串长*/
} SqString;
extern void StrAssign(SqString &, char[]); /*在algo4-1.cpp文件中*/
extern void DispStr(SqString);

SqString A, B;			/*全局串*/
SqString EnCrypt(SqString p)
{
	int i = 0, j;
	SqString q;
	while (i<p.len)
	{
		for (j = 0; p.ch[i] != A.ch[j]; j++);
		if (j >= A.len)			/*在A串中未找到p.ch[i]字母*/
			q.ch[i] = p.ch[i];
		else					/*在A串中找到p.ch[i]字母*/
			q.ch[i] = B.ch[j];
		i++;
	}
	q.len = p.len;
	return q;
}

SqString UnEncrypt(SqString q)
{
	int i = 0, j;
	SqString p;
	while (i<q.len)
	{
		for (j = 0; q.ch[i] != B.ch[j]; j++);
		if (j >= B.len)			/*在B串中未找到q.ch[i]字母*/
			p.ch[i] = q.ch[i];
		else					/*在B串中找到q.ch[i]字母*/
			p.ch[i] = A.ch[j];
		i++;
	}
	p.len = q.len;
	return p;
}

void TEST03_Sqstr_main()
{
	SqString p, q;
	int ok = 1;
	StrAssign(A, "abcdefghijklmnopqrstuvwxyz");	/*建立A串*/
	StrAssign(B, "ngzqtcobmuhelkpdawxfyivrsj");	/*建立B串*/
	char str[MaxSize];
	printf("\n");
	printf("输入原文串:");

	gets_s(str);									/*获取用户输入的原文串*/
	StrAssign(p, str);							/*建立p串*/
	printf("加密解密如下:\n");
	printf("  原文串:"); DispStr(p);

	q = EnCrypt(p);								/*p串加密产生q串*/
	printf("  加密串:"); DispStr(q);

	p = UnEncrypt(q);							    /*q串解密产生p串*/
	printf("  解密串:"); DispStr(p);
	printf("\n");
}
