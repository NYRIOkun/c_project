/* 共有メモリを用いて、コマンド入力したデータをLEDの点灯・消灯に使用する課題　*/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>

#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

int main(int argc, char *argv[])
{
	int fd;
	int i;
	key_t shmkey;
	int shmid;
	char LED;
	char *pos;

	if(argc < 3){
		printf("入力足りないよ\n");
		return 1;
	}
	switch(argv[1][0]){
		case 'r' :
			fd = open(R_LED,O_WRONLY);
			break;
		case 'g' :
			fd = open(G_LED,O_WRONLY);
			break;
		case 'y' :
			fd = open(Y_LED,O_WRONLY);
			break;
		default:
			printf("違う文字だよ\n");
			return 1;
			break;
	}
	
	if(fork()==0){
		// 共有メモリ取得して　argv[2]の内容を書き込み
		shmkey=ftok("shmtest3",'a');
		shmid=shmget(shmkey,sizeof(LED),IPC_CREAT | 0666);
		pos=shmat(shmid,0,0);
		
		if(argv[2][0] == '1' || argv[2][0] == '0'){
			strcpy(pos,argv[2]);
		}
		else{
		 	printf("１か0を入れてください\n");
			return 1;
		}
		shmdt(pos);
		_exit(0);
	}
	if(fork()==0){
		shmkey=ftok("shmtest3",'a');
		shmid=shmget(shmkey,sizeof(LED),IPC_CREAT | 0666);
		pos=shmat(shmid,0,0);
		sleep(1);
		write(fd,pos,1);
		shmdt(pos);
		_exit(0);
	}
}


