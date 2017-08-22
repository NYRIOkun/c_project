#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#define DEVNAME1 "/dev/input/event1"
#define FIFO "testfifo"

int main(int argc,char *argv[])
{
	int fd0,fd1;
	struct input_event ev;
	int xp,yp;
	int point;

	fd0=open(FIFO,O_WRONLY);
	fd1=open(DEVNAME1,O_RDONLY);
	while(1){
		read(fd1,&ev,sizeof(ev));
		//イベント終了
		switch(ev.type){			
			case EV_SYN:
				printf("EV_SYN\n");
				break;
			//イベント発生
			case EV_KEY:
				printf("EV_KEY code=%d value=%d\n",ev.code,ev.value);
				if(ev.value==1)
				{
					write(fd0,&point,sizeof(point));
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
	close(fd0);
	close(fd1);
	return 0;
}


