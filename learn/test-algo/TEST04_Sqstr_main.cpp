/*文件名:exp4-5.cpp*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
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

SqString *MaxSubstr(SqString s)
{
	SqString *sp;
	int index = 0, length = 0, length1, i = 0, j, k;
	while (i<s.len)
	{
		j = i + 1;
		while (j<s.len)
		{
			if (s.ch[i] == s.ch[j]) /*找一子串,其序号为i,长度为length1*/
			{
				length1 = 1;

				for (k = 1; s.ch[i + k] == s.ch[j + k]; k++)
					length1++;

				if (length1>length)    /*将较大长度者赋给index与length*/
				{
					index = i;
					length = length1;
				}
				j += length1;
			}
			else
				j++;
		}
		i++;                                 /*继续扫描第i字符之后的字符*/
	}

	sp = (SqString *)malloc(sizeof(SqString));
	sp->len = length;
	for (i = 0; i<length; i++)
		sp->ch[i] = s.ch[index + i];
	return sp;
}

void TEST04_Sqstr_main()
{
	char str[MaxSize];
	SqString s, *sp;
	printf("输入串:");

	gets_s(str);
	StrAssign(s, str);			/*创建串s*/
	sp = MaxSubstr(s);

	printf("求最长重复子串:\n");
	printf("  原串:        ");
	DispStr(s);
	printf("  最长重复子串:");	/*输出最长重复子串*/
	DispStr(*sp);
}
