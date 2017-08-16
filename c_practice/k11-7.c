#include <stdio.h>
#include <string.h>			//for strlen()
#include <time.h>
#include <unistd.h>
#include <fcntl.h>			//for open()
#include <stdlib.h>			//for rand()
#include <linux/input.h>
#include <sys/select.h>
#include <sys/stat.h>		//for stat()
#include <sys/mman.h>		//for mmap(),munmap()

//prottype

//origin define
#define error

//need define
#define SCREENWIDTH		480
#define SCREENHEIGHT	272
#define SCREENSIZE		(SCREENWIDTH * SCREENHEIGHT * 2)

#include "draw.h"


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
	//for mem
	int fd_mem;
	int x,y;
	unsigned short r,g,b;
	unsigned short *pfb;
	unsigned short *ppic;
	unsigned short color;

	
	//for led
	int fd_led;
	char buf[256];
	char devfile[40];
	int xp,yp;
	int point;
	
	
	//for touch panel
	int fd_scr;
	int ret;
	fd_set readfs;
	int maxfd;
	char pushed=1;
	
	//input struct
	struct input_event ev;
	

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
	//file open
 	fd_mem = open("/dev/fb0",O_RDWR);
	fd_led = open(DEVNAME0,O_RDONLY);
	fd_scr = open(DEVNAME1,O_RDONLY);
	
	maxfd=(fd_scr>fd_led)? fd_scr+1:fd_led+1;
	
	//mmapでバッファ先頭アドレスを取得
	pfb = mmap(0,SCREENSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd_mem,0);

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
	memset(pfb,0,SCREENSIZE);
	
	
	//init screen-status
	unsigned short color1=RGB(31,0,0);
	unsigned short color2=RGB(0,63,0);
	unsigned short color3=RGB(31,63,0);
	unsigned short color4=RGB(31,63,31);
	unsigned short tmp=0;
	
	lcd_drw_rec(pfb,0,0,240,135,	color1,1);
	lcd_drw_rec(pfb,241,0,479,135,	color2,1);
	lcd_drw_rec(pfb,0,136,241,270,	color3,1);
	lcd_drw_rec(pfb,241,13,479,270, color4,1);
	
	lcd_drw_rec(pfb,10,10,230,125,	color2,1);
	lcd_drw_rec(pfb,250,10,470,125,	color3,1);
	lcd_drw_rec(pfb,10,145,230,260,	color4,1);
	lcd_drw_rec(pfb,250,145,470,260,color1,1);
	//show sand-storm
	while(1){
		FD_SET(fd_led,&readfs);
		FD_SET(fd_scr,&readfs);
		select(maxfd,&readfs,NULL,NULL,NULL);
		if(FD_ISSET(fd_led,&readfs)){
			ret = read(fd_led,&ev,sizeof(ev));
		}
		if(FD_ISSET(fd_scr,&readfs)){
			ret = read(fd_scr,&ev,sizeof(ev));
		} 
		printf("%d:%d ",ev.time.tv_sec,ev.time.tv_usec);
		switch(ev.type){
			case EV_SYN:
				printf("EV_SYN\n");
				break;
			case EV_KEY:
				printf("EV_KEY code=%d value=%d\n",ev.code,ev.value);
				if(ev.value==1)
				{
					if(ev.code==158)
						point=1;
					if(ev.code==139)
						point=2;
					if(ev.code==102)
						point=3;
					if(ev.code==28)
						point=4;
				}	
				break;
			case EV_ABS:
				printf("EV_ABS code=%d value=%d\n",ev.code,ev.value);
				if(ev.code==0) //X座標(横方向）の時
					xp=ev.value;
				if(ev.code==1) //Y座標(縦方向）の時
					yp=ev.value;
				tmp	   = color1
				color1 = color2;
				color2 = color3;
				color3 = color4;
				color4 = tmp;
				break;
		}
		//write set-code under this line
		lcd_drw_rec(pfb,0,0,240,135,	color1,1);
		lcd_drw_rec(pfb,241,0,479,135,	color2,1);
		lcd_drw_rec(pfb,0,136,241,270,	color3,1);
		lcd_drw_rec(pfb,241,13,479,270, color4,1);
		
		lcd_drw_rec(pfb,10,10,230,125,	color2,1);
		lcd_drw_rec(pfb,250,10,470,125,	color3,1);
		lcd_drw_rec(pfb,10,145,230,260,	color4,1);
		lcd_drw_rec(pfb,250,145,470,260,color1,1);
		//set-end
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
	close(fd_led);
	close(fd_scr);
	close(fd_mem);
	return 0;
}


/****************************
 * =========================
 * show-set
 * =========================


1. bonobono
==========================
		switch(point){
			case 1:
				lcd_drw_cir(pfb,180,80,10,255,1);
				lcd_drw_cir(pfb,300,80,10,255,1);
				break;
			case 2:
				lcd_drw_cir(pfb,240,135,20,255,0);
				lcd_drw_cir(pfb,200,135,20,255,0);
				lcd_drw_cir(pfb,280,135,20,255,0);
				break;
			case 3:
				lcd_drw_cir(pfb,240,135,100,255,0);
				break;
			case 4:
				lcd_drw_rec(pfb,10,10,470,262,255,0);
				break;
			case 5:
				lcd_clr(pfb);
				break;
		}
==========================


2.change-color
==========================
		switch(point){
			case 1:
				color=RGB(0,0,0):
				break;
			case 2:
				color=RGB(31,0,0):
				break;
			case 3:
				color=RGB(0,63,0):
				break;
			case 4:
				color=RGB(31,63,0):
				break;
		}
		for(x=0; x<SCREENWIDTH;x++){
			for(y=0; y<SCREENHEIGHT;y++){
				ppic=pfb+x+y*SCREENWIDTH;
				*ppic=RGB(r,g,b);
			}
		}
==========================


3.code-temp(in case of EV_ABS code)
==========================
			case EV_ABS:
				printf("EV_ABS code=%d value=%d\n",ev.code,ev.value);
				if(ev.code==0) //X座標(横方向）の時
					xp=ev.value;
				if(ev.code==1) //Y座標(縦方向）の時
					yp=ev.value;
				if(xp<2000){
					if(yp<2000) point=1;
					else		point=2;
				}else{
					if(yp<2000) point=3;
					else		point=4;
				}
				switch(point){
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				}
==========================
*/