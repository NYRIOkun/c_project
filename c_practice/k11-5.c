#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>

#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

int main(int argc,char *argv[])
{
	int fd1;
	int fd2;
	char buf[256];
	char devfile[40];
	unsigned char r=0;
	unsigned char g=0;
	unsigned char y=0;
	char data[3];
	int xp,yp;
	int point;

	//インプットイベント構造体
	struct input_event ev;

	//タッチパネルデバイスファイルを読み込み用でオープン	
	if((fd1=open("/dev/input/event1",O_RDONLY))==-1){
		sprintf(buf,"%s:open():%s",argv[0],"/dev/input/event1");
		perror(buf);
		return 1;
	}

	while(1){
		//キーイベントの読み込み
		if(read(fd1,&ev,sizeof(ev))==-1){
			sprintf(buf,"%s:read()",argv[0]);
			perror(buf);
			close(fd1);
			return 1;
		}
		printf("%d.%d ",ev.time.tv_sec,ev.time.tv_usec);
		switch(ev.type){
			//イベント終了
			case EV_SYN:
				printf("EV_SYN\n");
				break;
			//イベント発生
			case EV_KEY:
				printf("EV_KEY code=%d value=%d\n",ev.code,ev.value);
				if(ev.value==1)
				{
					switch(point){
						case 1:
							strcpy(devfile,R_LED);
							r=r^1; //0->1  1->0 にする
							sprintf(data,"%d",r);
							break;
						case 2:
							strcpy(devfile,G_LED);
							g=g^1;
							sprintf(data,"%d",g);
							break;
						case 3:
							strcpy(devfile,Y_LED);
							y=y^1;
							sprintf(data,"%d",y);
							break;
					}	
					if((fd2=open(devfile,O_WRONLY))==-1){ //デバイスファイルを開く
						sprintf(buf,"%s:open():%s",argv[0],devfile);
						perror(buf);
						return 1;
					}
					if(write(fd2,data,1)==-1){  //LED_の制御
						sprintf(buf,"%s:write():%s",argv[0],devfile);
						perror(buf);
						return 1;
					}
					close(fd2);
				}		
				break;
			case EV_ABS:
				printf("EV_ABS code=%d value=%d\n",ev.code,ev.value);
				if(ev.code==0) //X座標(横方向）の時
					xp=ev.value;
				if(ev.code==1) //Y座標(縦方向）の時
					yp=ev.value;
				if(xp<1400)
					point=3;
				else if(xp<2700)
					point=2;
				else
					point=1;
				break;
		}
	}
	close(fd1);
	return 0;
}

