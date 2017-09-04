#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <tslib.h>
#define DEVNAME "/dev/input/event1"

#define SCR_WID 480
#define SCR_HIGH 272

void pset(unsigned short *pfb,int x,int y,unsigned short color);


int main(void)
{
    int ret;
    struct tsdev *ts;
    struct ts_sample samp;
    
    ts = ts_open(DEVNAME,0);
    ret = ts_config(ts);
    
    for(;;){
        ret = ts_read(ts,&samp,1);
        if(samp.x > 260){
            printf("右だよ");
        }else{
            printf("左だよ");
        }
        pset(ret,samp.x,samp.y,888);
        printf("x:%6d y:%6d p:%6d\n",samp.x,samp.y,samp.pressure); 
    }
    ts_close(ts);
    return 0;
}

void pset(unsigned short *pfb,int x,int y,unsigned short color)
{
    if(y*SCREENWIDTH+x < SCREENWIDTH*SCREENHEIGHT)
        *(pfb+y*SCREENWIDTH+x) = color;
}
