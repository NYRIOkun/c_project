#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

void tmh(int signum);
void led(char *devfile,char *data);

//sigjmp_buf jpenv;

unsigned char r=0;
unsigned char g=0;
unsigned char y=0;

int main(int argc,char *argv[])
{
	int fd1;
	char buf[256];
	char devfile[40];
	char data[3];

	struct sigaction sig={0};
	sig.sa_handler=tmh;
	//sig.sa_flags=SA_NODEFER; 
	sigaction(SIGALRM,&sig,NULL);

	//インプットイベント構造体
	struct input_event ev;

	//ボタンスイッチデバイスファイルを読み込み用でオープン	
	if((fd1=open("/dev/input/event0",O_RDONLY))==-1){
		sprintf(buf,"%s:open():%s",argv[0],"/dev/input/event0");
		perror(buf);
		return 1;
	}

	while(1){
		alarm(5);
//		sigsetjmp(jpenv,1);

		//キーイベントの読み込み
		if(read(fd1,&ev,sizeof(ev))==-1){
			if(errno == 4)
				continue;
			sprintf(buf,"%s errno=%d :read()",argv[0],errno);
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
					switch(ev.code){
						case 158:
							strcpy(devfile,R_LED);
							r=r^1; //0->1  1->0 にする
							sprintf(data,"%d",r);
							break;
						case 139:
							strcpy(devfile,G_LED);
							g=g^1;
							sprintf(data,"%d",g);
							break;
						case 102:
							strcpy(devfile,Y_LED);
							y=y^1;
							sprintf(data,"%d",y);
							break;
					}
					led(devfile,data);
				}		
				break;
		}
	}
	close(fd1);
	return 0;
}

void tmh(int signum)
{
	char buf[256];
	r=g=y=0;
	led(R_LED,"0");
	led(G_LED,"0");
	led(Y_LED,"0");
//	siglongjmp(jpenv,0);
}

void led(char *devfile,char *data)
{
	int fd;
	char buf[256];

	if((fd=open(devfile,O_WRONLY))==-1){ //デバイスファイルを開く
		sprintf(buf,"open():%s",devfile);
		perror(buf);
		_exit(1);
	}
	if(write(fd,data,1)==-1){  //LED_の制御
		sprintf(buf,"write():%s",devfile);
		perror(buf);
		_exit(1);
	}
	close(fd);
}
