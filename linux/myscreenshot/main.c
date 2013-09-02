/********************************************************************
	created:	2012/10/08
	filename: 	main.c
	author:		
	purpose:	
*********************************************************************/

//-------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include "myfb.h"

void usage() {
    fprintf(stderr,
            "usage: save [-p pathname] [-n filename]\n"
            "   -p: Specified file path.\n"
            "   -n: Specified file name.\n"
    );
}

int main(int argc, char * argv[])
{
	char *pathname = NULL;
	char *filename = NULL;
	int c;

	while ((c = getopt(argc, argv, "p:n:")) != -1)
	{
		switch(c) {
			case 'p': 
				pathname = optarg;
				printf("pathname: %s\n", pathname);
				break;
			case 'n': 
				filename = optarg;
				printf("filename: %s\n", filename);
				break;
			case '?':
			case 'h':
				printf("\n"); usage(); exit(1);
		}
	}
	
	if(argc < 2)
	{
		usage();
		pathname = "/mnt/sdcard/s.bmp";
		printf("Default Path: %s\n", pathname);
	}
	else
	{
		strcat(pathname, filename);
		printf("full-path: %s\n", pathname);
	}

	screen_shot(pathname);
	return 0;
}

//-------------------------------------------------------------------
