/********************************************************************
	created:	2012/10/08
	filename: 	savebmp.c
	author:		
	
	purpose:	
*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

/*
  (bitmap-file header) BITMAPFILEHEADER bmfh
  (bitmap-information header) BITMAPINFOHEADER bmih
  color table) RGBQUAD aColors[]
  BYTE aBitmapBits[]
*/
typedef struct bmp_header 
{
	//14B
	char bfType[2];				// 'BM'(0x4D42)
	unsigned long int bfSize;	// file size(byte)
	unsigned long int bfReserved; //reserved: 0
	unsigned long int bfOffBits; //14B+sizeof()
}__attribute__((packed)) BMPHEADER;

typedef struct bmp_info
{
	//40B
	unsigned long int biSize;   
	long int biWidth;
	long int biHeight;
	unsigned short int biPlanes;
	unsigned short int biBitCount;
	unsigned long int biCompression;
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
	unsigned long int biSizeImage;
	long int biXPelsPerMeter;
	long int biYPelsPerMeter;
	unsigned long int biClrUsed;
	unsigned long int biClrImportant;
}__attribute__((packed)) BMPINFO;

typedef struct tagRGBQUAD {
	unsigned short rgbBlue;    
	unsigned short rgbGreen;
	unsigned short rgbRed;
	unsigned short rgbReserved;
}__attribute__((packed)) RGBQUAD;

typedef struct tagBITMAPINFO {
    BMPINFO    bmiHeader;
//	RGBQUAD    bmiColors;
	unsigned long int rgb[3];
}__attribute__((packed)) BITMAPINFO;

static int get_rgb888_header(int w, int h, BMPHEADER * head, BMPINFO * info)
{
	int size = 0;
	if (head && info) {
		size = w * h * 3;
		memset(head, 0, sizeof(* head));
		memset(info, 0, sizeof(* info));
		head->bfType[0] = 'B';
		head->bfType[1] = 'M';
		head->bfOffBits = 14 + sizeof(* info);
		head->bfSize = head->bfOffBits + size;
		head->bfSize = (head->bfSize + 3) & ~3;
		size = head->bfSize - head->bfOffBits;
		
		info->biSize = sizeof(BMPINFO);
		info->biWidth = w;
		info->biHeight = -h;
		info->biPlanes = 1;
		info->biBitCount = 24;
		info->biCompression = BI_RGB;
		info->biSizeImage = size;

		printf("rgb888:%dbit,%d*%d,%lu\n", info->biBitCount, w, h, head->bfSize);
	}
	return size;
}

static int get_rgb565_header(int w, int h, BMPHEADER * head, BITMAPINFO * info)
{
	int size = 0;
	if (head && info) {
		size = w * h * 2;
		memset(head, 0, sizeof(* head));
		memset(info, 0, sizeof(* info));
		head->bfType[0] = 'B';
		head->bfType[1] = 'M';
		head->bfOffBits = 14 + sizeof(* info);
		head->bfSize = head->bfOffBits + size;
		head->bfSize = (head->bfSize + 3) & ~3; // 4的倍数 
		size = head->bfSize - head->bfOffBits;
		
		info->bmiHeader.biSize = sizeof(info->bmiHeader);
		info->bmiHeader.biWidth = w;
		info->bmiHeader.biHeight = -h;
		info->bmiHeader.biPlanes = 1;
		info->bmiHeader.biBitCount = 16;
		info->bmiHeader.biCompression = BI_BITFIELDS;
		info->bmiHeader.biSizeImage = size;
		info->bmiHeader.biXPelsPerMeter = w;
		info->bmiHeader.biYPelsPerMeter = h;
		info->bmiHeader.biClrUsed = 0;
		info->bmiHeader.biClrImportant = 0;

		info->rgb[0] = 0xF800; // blue
		info->rgb[1] = 0x07E0; // green
		info->rgb[2] = 0x001F; // red
		info->rgb[3] = 0x0000; // reserved
//		info->bmiColors.rgbBlue = 0xF800;
//		info->bmiColors.rgbGreen = 0x07E0;
//		info->bmiColors.rgbRed = 0x001F;
//		info->bmiColors.rgbReserved = 0;

		printf("rgb565:%dbit,%d*%d,%lu\n", info->bmiHeader.biBitCount, w, h, head->bfSize);
	}
	return size;
}

static int save_bmp_rgb565(FILE * hfile, int w, int h, void * pdata)
{
	int success = 0;
	int size = 0;
	BMPHEADER head;
	BITMAPINFO info;
	
	size = get_rgb565_header(w, h, &head, &info);
	if (size > 0) {
		fwrite(head.bfType, 1, 14, hfile);
		fwrite(&info, 1, sizeof(info), hfile);
		fwrite(pdata, 1, size, hfile);
		success = 1;
	}

	return success;
}

static int save_bmp_rgb888(FILE * hfile, int w, int h, void * pdata)
{
	int success = 0;
	int size = 0;
	BMPHEADER head;
	BMPINFO info;
	
	size = get_rgb888_header(w, h, &head, &info);
	if (size > 0) {
		fwrite(head.bfType, 1, 14, hfile);
		fwrite(&info, 1, sizeof(info), hfile);
		fwrite(pdata, 1, size, hfile);
		success = 1;
	}
	
	return success;
}

int save_bmp(const char * path, int w, int h, void * pdata, int bpp)
{
	int success = 0;
	FILE * hfile = NULL;

	do 
	{
		if (path == NULL || w <= 0 || h <= 0 || pdata == NULL) {
			printf("if (path == NULL || w <= 0 || h <= 0 || pdata == NULL)\n");
			break;
		}

		remove(path);
		hfile = fopen(path, "wb");
		if (hfile == NULL) {
			printf("open(%s) failed!\n", path);
			break;
		}

		switch (bpp)
		{
		case 16:
			success = save_bmp_rgb565(hfile, w, h, pdata);
			break;
		case 24:
			success = save_bmp_rgb888(hfile, w, h, pdata);
			break;
		default:
			printf("error: not support format!\n");
			success = 0;
			break;
		}
	} while (0);

	if (hfile != NULL)
		fclose(hfile);
	
	return success;
}
//-------------------------------------------------------------------
