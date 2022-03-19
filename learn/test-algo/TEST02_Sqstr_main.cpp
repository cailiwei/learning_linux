/*文件名:exp4-3.cpp*/
#include <stdio.h>
#include <string.h>

#define MaxSize 100
typedef struct
{
	char ch[MaxSize];     			/*定义可容纳MaxSize个字符的空间*/
	int len;             			/*标记当前实际串长*/
} SqString;

extern void StrAssign(SqString &, char[]); /*在algo4-1.cpp文件中*/
extern void DispStr(SqString);

int Index(SqString s, SqString t)	/*简单匹配算法*/
{
	int i = 0, j = 0, k;
	while (i<s.len && j<t.len)
	{
		if (s.ch[i] == t.ch[j])  	/*继续匹配下一个字符*/
		{
			i++; j++;
		}
		else          			/*主串、子串指针回溯重新开始下一次匹配*/
		{
			i = i - j + 1; j = 0;
		}
	}
	if (j >= t.len)
		k = i - t.len;				/*返回匹配的第一个字符的下标*/
	else
		k = -1;        			/*模式匹配不成功*/
	return k;
}

void GetNext(SqString t, int next[]) /*由模式串t求出next值*/
{
	int j, k;
	j = 0; k = -1; next[0] = -1;
	while (j < t.len - 1)
	{
		if (k == -1 || t.ch[j] == t.ch[k])
		{
			j++; k++;
			next[j] = k;
		}
		else  k = next[k];
	}
}

void GetNextval(SqString t, int nextval[])  /*由模式串t求出nextval值*/
{
	int j = 0, k = -1;
	nextval[0] = -1;
	while (j<t.len)
	{
		if (k == -1 || t.ch[j] == t.ch[k])
		{
			j++; k++;
			if (t.ch[j] != t.ch[k])
				nextval[j] = k;
			else
				nextval[j] = nextval[k];
		}
		else  k = nextval[k];
	}
}

int KMPIndex(SqString s, SqString t)  /*KMP算法*/
{
	int next[MaxSize], i = 0, j = 0, v;

	GetNext(t, next);

	while (i<s.len && j<t.len)
	{
		if (j == -1 || s.ch[i] == t.ch[j])
		{
			i++; j++;
		}						/*i,j各增1*/
		else j = next[j]; 		/*i不变,j后退*/
	}
	if (j >= t.len)
		v = i - t.len;  			/*返回匹配模式串的首字符下标*/
	else
		v = -1;        			/*返回不匹配标志*/
	return v;
}

int KMPIndex1(SqString s, SqString t)    /*改进的KMP算法*/
{
	int nextval[MaxSize], next[MaxSize], i = 0, j = 0, v;

	GetNextval(t, next);
	GetNextval(t, nextval);

	while (i<s.len && j<t.len)
	{
		if (j == -1 || s.ch[i] == t.ch[j])
		{
			i++; j++;
		}
		else j = nextval[j];
	}

	if (j >= t.len)
	{
		v = i - t.len;  			/*返回匹配模式串的首字符下标*/
	}
	else 
	{
		v = -1;
	}

	return v;
}

void TEST02_Sqstr_main()
{
	int j;
	int next[MaxSize], nextval[MaxSize];
	SqString s, t;
	StrAssign(s, "abcabcdabcdeabcdefabcdefg");
	StrAssign(t, "abcdeabcdefab");
	printf("串s:"); DispStr(s);
	printf("串t:"); DispStr(t);

	printf("简单匹配P算法:\n");
	printf("t在s中的位置=%d\n", Index(s, t));

	GetNext(t, next);			/*由模式串t求出next值*/
	GetNextval(t, nextval);		/*由模式串t求出nextval值*/
	printf("   j   ");
	for (j = 0; j<t.len; j++)
		printf("%4d", j);
	printf("\n");

	printf(" t[j]   ");
	for (j = 0; j<t.len; j++)
		printf("%4c", t.ch[j]);
	printf("\n");

	printf(" next   ");
	for (j = 0; j<t.len; j++)
		printf("%4d", next[j]);
	printf("\n");

	printf(" nextval");
	for (j = 0; j<t.len; j++)
		printf("%4d", nextval[j]);
	printf("\n");

	printf("KMP算法:\n");
	printf("  t在s中的位置=%d\n", KMPIndex(s, t));
	printf("改进的KMP算法:\n");
	printf("  t在s中的位置=%d\n", KMPIndex1(s, t));
}