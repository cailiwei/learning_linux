#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

typedef char ElemType;

typedef struct Node {
	ElemType data;
	struct Node *prev;
	struct Node *next;
} DLinkList;

void CreateListF(DLinkList **L, ElemType *num, int length)
{
	DLinkList *s;
	int i;

	*L = (DLinkList *)malloc(sizeof(DLinkList));
	(*L)->prev = (*L)->next = NULL;

	for (i = 0; i < length; i++)
	{
		s = (DLinkList *)malloc(sizeof(DLinkList));
		s->data = num[i];
		s->next = (*L)->next;
		if ((*L)->next != NULL)
			(*L)->next->prev = s;
		(*L)->next = s;
		s->prev = *L;
	}
}

void CreateListR(DLinkList **L, ElemType *num, int length)
{
	DLinkList *s, *r;
	*L = (DLinkList *)malloc(sizeof(DLinkList));
	(*L)->next = (*L)->prev = NULL;
	r = *L;
	for (int i = 0; i < length; i++)
	{
		s = (DLinkList *)malloc(sizeof(DLinkList));
		s->data = num[i];
		r->next = s;
		s->prev = r;
		r = s;
	}
	r->next = NULL;
}


void DispList(DLinkList *L)
{
	DLinkList *p = L->next;
	while (p != NULL)
	{
		printf(" %c ", p->data);
		p = p->next;
	}
	printf("\n");
}

int ListLength(DLinkList *L)
{
	DLinkList *p = L;
	int n = 0;
	while (p->next != NULL) {
		n++;
		p = p->next;
	}
	return n;
}

void DestroyList(DLinkList *L)
{
	DLinkList *p = L, *q = p->next;
	while ( q != NULL) {
		free(p);
		p = q;
		q = p->next;
	}
	free(p);
}

int lengthOfLongestSubstring(char *s)
{
	DLinkList *L = NULL;
	CreateListR(&L, s, strlen(s));
	DispList(L);
	// DLinkList *p = L;
	// char chr = L->data;
	// int cnt = 0;

	DestroyList(L);

	return 0;
}

int minHeight(int** books, int booksSize, int width)
{
	int minCol = 0;
	
	if (width == 1) {
		for (int i = 0; i < booksSize; i++) {
			minCol = minCol + books[i][1];
		}
	} else {
		int bookWidth = 0;
		int maxWidth = 0;
		for (int i = 0; i < booksSize; i++) {
			bookWidth += books[i][0];
			if (bookWidth < width) {
				maxWidth = maxWidth > books[i][1] ? maxWidth : books[i][1];
			} else {
				bookWidth = 0;
				minCol += maxWidth;
			}
		}
		
		int minBook = minHeight(books, booksSize, width - 1);
		minCol = minCol > minBook ? minBook : minCol;
	}
	return minCol;
}

int minHeightShelves(int** books, int booksSize, int* booksColSize, int shelf_width)
{
	int minCol = 0;
	printf("booksSize = %d\n", booksSize);
	for (int i = 0; i < booksSize; i++) {
		printf(" [%d %d] ", books[i][0], books[i][1]);
		minCol = minCol + books[i][1];
	}
	printf("\n");
	printf("booksColSize = %d\n", *booksColSize);
	printf("shelf_width = %d\n", shelf_width);
	
	return minHeight(books, booksSize, shelf_width);
}

int strStr(char *haystack, char *needle)
{
	if (!needle || !*needle) {
		return 0;
	}

	if (!*haystack && !*needle) {
		return 0; 
	}

	char *s1 = haystack;
	char *s2 = needle;
	int num = 0;
	
	while (*haystack) {
		while (*s1++ == *s2++) {
			if (!*s2)
				return num;
			if (!*s1)
				return -1;
		}
		num++;
		haystack++;
		s1 = haystack;
		s2 = needle;
	}
	return -1;
}

int main(void)
{
	int book[][2] = {{1,1},{2,3},{2,3},{1,1},{1,1},{1,1},{1,2}};
	int booksSize = 7;
	int bookColSize = 2;
	int shelf_width = 4;
	int **p = NULL;

	p = (int **)malloc(booksSize * sizeof(int *));

	for (int i = 0; i < booksSize; i++ ) {
		p[i] = (int *)malloc(bookColSize * sizeof(int));
		for (int j = 0; j < bookColSize; j++) {
			p[i][j] = book[i][j];
		}
	}

	char *haystack = "hello", *needle = "e";

	printf("min = %d, num = %d \n", minHeightShelves(p, booksSize, &bookColSize, shelf_width), strStr(haystack, needle));
}