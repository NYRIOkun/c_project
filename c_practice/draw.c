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

#define SCREENWIDTH		480
#define SCREENHEIGHT	272
#define SCREENSIZE		(SCREENWIDTH * SCREENHEIGHT * 2)

#define RGB(r,g,b) (((r) << 11) | ((g) << 5) | (b))

#define DEVNAME0 "/dev/input/event0"
#define DEVNAME1 "/dev/input/event1"
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"


//指定した座標値を指定した色にする
void lcd_put_pnt(unsigned short *pfb,int x,int y,unsigned short color)
{
    unsigned short *ppic;
    ppic = pfb+x+y*SCREENWIDTH;
	*ppic = color;
}

//線分の描写
void lcd_drw_lin(unsigned short *pfb,int x1,int y1,int x2,int y2,unsigned color)
{
    
}

//短刑の描写
void lcd_drw_rec(unsigned short *pfb,int x1,int y1,int x2,int y2,unsigned short int fil)
{
    
}

//円の描写
void lcd_drw_cir(unsigned short *pfb,int x0,int y0,int r,unsigned short color,int fil)
{
    
}

//楕円の描写
void lcd_drw_ell(unsigned short *pfb,int x0,int y0,int a,int b,unsigned short color,int fil)
{
    
}

//画面クリア
void lcd_clr(unsigned short *pfb)
{
    memset(pfb,0,SCREENSIZE);
    return 0;
}