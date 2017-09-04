/* �^�b�`�������摜���؂��ւ����v���O���� */

#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <tslib.h>
#include "jpeg.h"

#define DEVFB "/dev/fb0"
#define SCREENWIDTH	480
#define SCREENHEIGHT	272
#define SCREENSIZE	(SCREENWIDTH * SCREENHEIGHT * 2)

char *bmpfile[]={NULL,
		"01.jpg",
		"02.jpg",
		"03.jpg",
		"04.jpg",
		"05.jpg",
		NULL};

int main(int argc,char *argv[])
{
	int fd;
	int ret;
	unsigned short *pfb;
	char buf[256];
	int bmp_n=0;
	int flg = 0;

	struct tsdev *ts;
	struct ts_sample samp;

	ts = ts_open("/dev/input/event1", 0);
	ret = ts_config(ts);

	fd=open(DEVFB,O_RDWR);
	if (fd<0) {
		perror("open(fb)");
		return 1;
	}
	pfb = mmap(0, SCREENSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (pfb == MAP_FAILED){
		perror("mmap");
		close(fd);
		return 1;
	}
	memset(pfb, 0, SCREENSIZE);
	while(1){
		ret = ts_read(ts, &samp, 5);
		if(samp.pressure>0 && flg==0){
			flg = 1;
			if(samp.x>240){
				bmp_n++;
				if(bmpfile[bmp_n]==NULL)bmp_n=1;
				printf("=> up num \n");
			}else{
				bmp_n--;
				if(bmpfile[bmp_n]==NULL)bmp_n=5;
				printf("=> down num \n");
			} 
			load_jpeg(bmpfile[bmp_n],pfb,SCREENWIDTH,SCREENHEIGHT);
			printf("file is %d\n",bmp_n);
		}
		else{
			flg = 0;
		}
		//sleep(1);
	}
	close(fd);
	return 0;
}

