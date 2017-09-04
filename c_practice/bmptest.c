#include <stdio.h>
#include <string.h>			//for strlen()
#include <fcntl.h>			//for open()
#include <sys/mman.h>		//for mmap(),munmap()
#include "bitmap.h"

//prottype

//need define
#define SCREENWIDTH		480
#define SCREENHEIGHT	272
#define SCREENSIZE		(SCREENWIDTH * SCREENHEIGHT * 2)
#define DEVFB 		"/dev/fb0"

int main(int argc,char *argv[])
{
	int fd;
	int ret;
	char buf[256];
	char bmpfile[]={"argv[argc-1]"}
	unsigned short *pfb;
    
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
		memset(pfb,0,SCREENSIZE);
		load_bmp(argv[argc-1] , pfb , SCREENWIDTH , SCREENHEIGHT);
		sleep(5);
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