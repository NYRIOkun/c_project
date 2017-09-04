#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <linux/input.h>
#include <tslib.h>

#define DEVNAME "/dev/input/event1"

#define SCREENWIDTH	480
#define SCREENHEIGHT	272
#define SCREENSIZE	(SCREENWIDTH * SCREENHEIGHT * 2)

#define RGB(r,g,b) (((r) << 11) | ((g) << 5) | (b))

/* 点描画関数 */
void pset(unsigned short *pfb,int x,int y,unsigned short color)
{
	if(y*SCREENWIDTH+x < SCREENWIDTH*SCREENHEIGHT)
		*(pfb+y*SCREENWIDTH+x) = color;
}

int main(void)
{
	int fd;
	int x,y;
	unsigned short r,g,b;
	unsigned short *pfb;
	unsigned short *ppic;
	int  ret;
	struct  tsdev  *ts;
	struct  ts_sample  samp;

	ts = ts_open(DEVNAME, 0);
	ret = ts_config(ts);

	fd = open("/dev/fb0", O_RDWR);
	// mmapによりバッファの先頭アドレスを取得/
	pfb = mmap(0, SCREENSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	// pfbをベースとしてメモリアクセス
	// 画面を黒で塗りつぶし
	memset(pfb, 0, SCREENSIZE);

	while(1){
		ret = ts_read(ts, &samp, 1);
		//printf("x:%6d y:%6d p:%6d\n", samp.x, samp.y, samp.pressure);
		if(samp.pressure>0)
			pset(pfb, samp.x, samp.y, RGB(31,63,31) );
	}
	ts_close(ts);
	munmap(pfb, SCREENSIZE);
	close(fd);

	return 0;
}

