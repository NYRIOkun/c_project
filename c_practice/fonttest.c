#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "font.h"

#define DEVFB "/dev/fb0"
#define FONTFILE  "/opt/fonts/ume-pgc4.ttf"

#define SCREENWIDTH	480
#define SCREENHEIGHT	272
#define SCREENSIZE	(SCREENWIDTH * SCREENHEIGHT * 2)
#define RGB(r,g,b) (((r) << 11) | ((g) << 5) | (b))

int main(int argc,char *argv[])
{
	int fd;
	int ret,i,w,x=10,z=0;
	unsigned short *pfb;
	unsigned char buf[256];
	unsigned char str[]="Apple";


	fd=open(DEVFB,O_RDWR);
	if (fd<0) {
		perror("open(fb)");
		return 1;
	}
	pfb = mmap(0, SCREENSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (pfb == MAP_FAILED){
		perror("mmap");
		return 1;
	}
	memset(pfb, 0, SCREENSIZE);
	for(i=0;i<strlen(str);i++){
		w = put_char(pfb,FONTFILE,str[i],50,x,30,
			RGB(31,63,31),RGB(31,63,31));
		x += w +5;
	}
	close(fd);
	return 0;
}

