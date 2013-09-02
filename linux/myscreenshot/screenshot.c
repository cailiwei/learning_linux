/********************************************************************
	created:	2012/10/08
	filename: 	screenshot.c
	author:		
	purpose:	
*********************************************************************/

#include <stdlib.h>
#include <memory.h>
#include "myfb.h"

int screen_shot(const char *path)
{
	struct FB * fb = NULL;
	fb = fb_create();
	if (fb) {
		save_bmp(path, fb_width(fb), fb_height(fb), fb_bits(fb), fb_bpp(fb));
		fb_destory(fb);
	}
	return 0;
}
