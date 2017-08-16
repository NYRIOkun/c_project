#include <stdio.h>
#include <string.h>			//for strlen()
//#include <time.h>
#include <fcntl.h>			//for open()
#include <stdlib.h>			//for rand()
//#include <linux/input.h>
//#include <sys/select.h>
//#include <sys/stat.h>		//for stat()
#include <sys/mman.h>		//for mmap(),munmap()

//prottype
int error_func(char *buf,char *text);

//origin define
#define error

//need define
#define SCREENWIDTH		480
#define SCREENHEIGHT	272
#define SCREENSIZE		(SCREENWIDTH * SCREENHEIGHT * 2)


//color-array formatting [RED(5bit)|GREEN(6bit)|BLUE(5bit)]
#define RGB(r,g,b) (((r) << 11) | ((g) << 5) | (b))

#define DEVNAME0 "/dev/input/event0"
#define DEVNAME1 "/dev/input/event1"
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"


//get line of argc
int main(int argc,char *argv[])
{
	int fd;
	int x,y;
	unsigned short r,g,b;
	unsigned short *pfb;
	unsigned short *ppic;
	
	fd= open("/dev/fb0",O_RDWR);

	
/*********************************
 * ===============================
 * mmap(*<start>,<length>,<prot>,<flags>,<fd>,<offset>)
 * ===============================
 * usage			: fileの情報をメモリにマッピングする
 * <start>	 		: 始まりのメモリアドレス(NULLでシステム指定)
 * <length>			: アドレスの長さ指定
 * <fd>				: 格納先のファイル
 * <offset>			: 終端アドレス直後の、NULLメモリ数の設定
 * return			: success:1、error:0
 * other			: おまじない
 * ==============================
 ********************************/
	//mmapでバッファ先頭アドレスを取得
	pfb = mmap(0,SCREENSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

/*********************************
 * ===============================
 * memset(*<target>,<data>,<size>)
 * ===============================
 * usage			: size分メモリアクセスでtargetする
 * <target>			: target先の先頭アドレス
 * <deta>			: 書き込むdata
 * <size>			: size
 * return			: OK:1 ER:0
 * ==============================
 ********************************/
	//pfbを基準にしてメモリアクセス
	memset(pfb,255,SCREENSIZE);
	
	//show sand-storm
	for(x=0; x<SCREENWIDTH;x++){
		for(y=0; y<SCREENHEIGHT;y++){
			r=rand()%32;
			g=rand()%64;
			b=rand()%32;
			ppic=pfb+x+y*SCREENWIDTH;
			*ppic=RGB(r,g,b);
			//pfb[x+y*SCREENWIDTH]= RGB(r,g,b);
		}
	}
/*********************************
 * ===============================
 * munmap(*<target>,<size>)
 * ===============================
 * usage			: メモリマッピングをアンマウントする
 * <target>	 		: 解除するMAPを指定
 * <size>			: 長さ指定
 * return			: OK:1 ER:0
 * ==============================
 ********************************/
	munmap(pfb,SCREENSIZE);
	close(fd);
	return 0;
}