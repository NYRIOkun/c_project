#include <stdio.h>
#include <string.h>			//for strlen()
#include <fcntl.h>			//for open()
#include <sys/mman.h>		//for mmap(),munmap()
#include <linux/input.h>
#include <tslib.h>
#include "bitmap.h"

//prottype

//need define
#define SCREENWIDTH		480
#define SCREENHEIGHT	272
#define SCREENSIZE		(SCREENWIDTH * SCREENHEIGHT * 2)
#define DEVFB 		"/dev/fb0"
#define DEVNAME "/dev/input/event1"

int main(void)
{
	int fd;
	int ret;
    int i=0,flg=0;
    struct tsdev *ts;
    struct ts_sample samp;
	char buf[256];
	char *bmpfile[]={"tanuki1.bmp",
					 "tanuki2.bmp",
					 "tanuki3.bmp",
					 "tanuki4.bmp",
					NULL};
					
	unsigned short *pfb;
     
    ts = ts_open(DEVNAME,0);
    ret = ts_config(ts);
	fd  = open(DEVFB,O_RDWR);	//画面描写のファイルを開く
	
	if(fd<0) {
		perror("open(fb)");
		return 1;
	}
	
	/** mmapでバッファ先頭アドレスを取得 **/
	pfb = mmap(0,SCREENSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(pfb == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 1;
	}
	//pfbを基準にしてメモリアクセス
	while(1){
		ret = ts_read(ts, &samp, 1);
		if(samp.pressure>0){
			flg=1;
			if(samp.x > 260){
				i++;
				if(bmpfile[i]==NULL)i=0;
				
	        }else{
				i--;
				if(i<0)i=3;
	        }
	        memset(pfb,0,SCREENSIZE); //これは白で塗りつぶしてますね 255=白
	        printf("x:%6d y:%6d p:%6d\n",samp.x,samp.y,samp.pressure); 
			load_bmp(bmpfile[i],pfb,SCREENWIDTH,SCREENHEIGHT);
		}else{
			flg=0;
		}
	}
	close(fd);
	return 0;
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