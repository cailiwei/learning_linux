#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static void stringConvert(unsigned char *src, unsigned char *dst) 
{
	char *rt = dst;
	char *ps = src;

	char *temp[10];
	int cnt = 0;
	int lenth = strlen(src);
	int i=0, j=0, num=0;

	temp[cnt++] = ps;
	while (lenth > 0) {
		if (*ps++ == '.') {
			temp[cnt++] = ps;
			num = 0;
		} else {
			num++;
		}
		lenth--;
	}

	cnt--;
	ps = ps - num;

	do {
		j = strlen(temp[cnt]);
		while ((j--) && (*(temp[cnt]) != '.')) {
			rt[i++] = *(temp[cnt]++);
		}
		rt[i++] = '.';
	} while ((--cnt) >= 0);

	rt[--i] = '\0';
}

void main()
{
	unsigned char *A = "182.89.80.100";
	unsigned char B[20];
	unsigned char *C = NULL;
	printf("A:%s\n", A);

	stringConvert(A, B);
	printf("B:%s\n", B);

	stringConvert(A, C);
	printf("C:%s\n", C);
}
