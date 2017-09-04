#include <stdio.h>
#include <string.h>			//for strlen()
#include <fcntl.h>			//for open()
#include <stdlib.h>			//for rand()
#include <tslib.h>
#include <linux/input.h>
#include <sys/mman.h>		//for mmap(),munmap()

//prottype
void pset(unsigned short *pfb,int x,int y,unsigned short color);

//need define
#define SCREENWIDTH		480
#define SCREENHEIGHT	272
#define SCREENSIZE		(SCREENWIDTH * SCREENHEIGHT * 2)
#define DEVNAME 		"/dev/input/event1"

//color-array formatting [RED(5bit)|GREEN(6bit)|BLUE(5bit)]
#define RGB(r,g,b) (((r) << 11) | ((g) << 5) | (b))

int main(void)
{
	int fd;
	int x,y;
	unsigned short r,g,b;
	unsigned short *pfb;
	unsigned short *ppic;
	int ret;
    struct tsdev *ts;
    struct ts_sample samp;
	
	ts  = ts_open(DEVNAME,0);		//タッチパネルを開く
    ret = ts_config(ts);			//設定を読み込む
    
	fd  = open("/dev/fb0",O_RDWR);	//画面描写のファイルを開く
	
	/** mmapでバッファ先頭アドレスを取得 **/
	pfb = mmap(0,SCREENSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	
	//pfbを基準にしてメモリアクセス
	memset(pfb,0,SCREENSIZE); //これは白で塗りつぶしてますね 255=白
	
	//show sand-storm
	for(;;){
		ret = ts_read(ts,&samp,1);
		printf("x:%6d y:%6d p:%6d\n", samp.x, samp.y, samp.pressure);
        if(samp.pressure > 0){
             pset(pfb, samp.x, samp.y, RGB(31,63,31) );
        }
	}
	
	ts_close(ts);
	munmap(pfb,SCREENSIZE);
	close(fd);
	return 0;
}

/** ペンでお絵かきできる関数 **/
void pset(unsigned short *pfb,int x,int y,unsigned short color)
{
    if(y*SCREENWIDTH+x < SCREENWIDTH * SCREENHEIGHT)
        *(pfb+y*SCREENWIDTH+x) = color;
}


/* ===============================
 * mmap(*<start>,<length>,<prot>,<flags>,<fd>,<offset>)
 * ===============================
 * usage			: fileの情報をメモリにマッピングする
 * <start>	 		: 始まりのメモリアドレス(NULLでシステム指定)
 * <length>			: アドレスの長さ指定
 * <fd>				: 格納先のファイル
 * <offset>			: 終端アドレス直後の、NULLメモリ数の設定
 * return			: success:1、error:0
 * other			: おまじない
 * ===============================
 
 * ===============================
 * memset(*<target>,<data>,<size>)
 * ===============================
 * usage			: size分メモリアクセスでtargetする
 * <target>			: target先の先頭アドレス
 * <data>			: 書き込むdata
 * <size>			: size
 * return			: OK:1 ER:0
 * ===============================
 
 * ===============================
 * munmap(*<target>,<size>)
 * ===============================
 * usage			: メモリマッピングをアンマウントする
 * <target>	 		: 解除するMAPを指定
 * <size>			: 長さ指定
 * return			: OK:1 ER:0
 * =============================== */