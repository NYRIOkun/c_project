#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#define DEVNAME0 "/dev/input/event0"
#define FIFO "testfifo"

int main(int argc,char *argv[])
{
	int fd0,fd1;
	struct input_event ev;
	int point;
	
	fd0=open(FIFO,O_WRONLY);	
	fd1=open(DEVNAME0,O_RDONLY);
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
					switch(ev.code){
						case 158:
							point=1;
							break;
						case 139:
							point=2;
							break;
						case 102:
							point=3;
							break;
						default:
							point=0;
					}	
					write(fd0,&point,sizeof(point));
				}
				break;
		}
	}
	close(fd0);
	close(fd1);
	return 1;
}
