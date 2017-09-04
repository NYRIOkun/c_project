#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>
#include <sys/time.h>

#include <linux/input.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

#define DEVNAME0 "/dev/input/event0"

void tmh(int signum);
void killchild(int signum);

pid_t i1,i2;

int main(int argc, char *argv[])
{
	int fd;
	char buf[256];

	struct sigaction sig={0};
	struct sigaction sig2={0};
	struct itimerval val;

	struct input_event ev;
	key_t shmkey;
	int shmid;
	int data;
	int *pos;
	int i;
	int st;

	if((i1=fork())==0){
		shmkey=ftok("k14-7",'a'); //kye生成  shmemはこのプロセスのプログラムファイル名
		shmid=shmget(shmkey,sizeof(data),IPC_CREAT | 0666); //共有メモリの生成・取得
		pos=shmat(shmid,0,0); //共有メモリにアタッチ
		*pos=1;
		fd=open(DEVNAME0,O_RDONLY);
		while(1){
			read(fd,&ev,sizeof(ev));
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
								*pos=1;
								break;
							case 139:
								*pos=2;
								break;
							case 102:
								*pos=3;
								break;
							default:
								*pos=4;
						}	
					}
					break;
			}
		}
		shmdt(pos); //共有メモリをデタッチ
		close(fd);
		_exit(1);
	}

	if((i2=fork())==0){
		shmkey=ftok("k14-7",'a'); //kye生成  shmemはこのプロセスのプログラムファイル名
		shmid=shmget(shmkey,sizeof(data),IPC_CREAT | 0666); //共有メモリの生成・取得
		pos=shmat(shmid,0,0); //共有メモリにアタッチ

		sig.sa_handler=tmh;//ハンドラ関数を設定
		sigaction(SIGALRM,&sig,NULL);//シグナルALRMを受けたら、関数tmhを呼び出す。
		val.it_interval.tv_sec=val.it_value.tv_sec=0;
		val.it_interval.tv_usec=val.it_value.tv_usec=500000;
		setitimer(ITIMER_REAL,&val,NULL);

		if((fd=open(Y_LED,O_WRONLY))==-1){
			sprintf(buf,"%s:open():%s",argv[0],Y_LED);
			perror(buf);
			return 1;
		}
		while (1){
			if(write(fd,"1",1)==-1){  //LED_ON
				sprintf(buf,"%s:write():%s",argv[0],Y_LED);
					perror(buf);
				return 1;
			}
			for(i=0;i<*pos;i++)
				pause();
			if(write(fd,"0",1)==-1){  //LED_OFF
				perror("led_blink:write():");
				return 1;
			}
			for(i=0;i<*pos;i++)
				pause();
		}
		shmdt(pos); //共有メモリをデタッチ
		close(fd);
		_exit(1);
	}

	sig.sa_handler=killchild;//ハンドラ関数を設定
	sigaction(SIGINT,&sig,NULL);//シグナルINTを受けたら、関数killchildを呼び出す。
	wait(&st);
	kill(i1,15);
	kill(i2,15);
	return 0;
}

void tmh(int signum)
{
	return;
}

void killchild(int signum)
{
	kill(i1,15);
	kill(i2,15);
	_exit(1);
}

