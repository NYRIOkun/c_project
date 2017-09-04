#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define DEVNAME0 "/dev/input/event0"
#define DEVNAME1 "/dev/input/event1"
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"
#define FIFO "testfifo"
#define LOG "k14.log"

int sem_lock(int semid);
int sem_unlock(int semid);

void killchild(int signum);

pid_t i1,i2,i3,i4;


int main(int argc,char *argv[])
{
	int fd0,fd1;
	struct input_event ev;
	char buf[256];
	char devfile[40];
	unsigned char r=0;
	unsigned char g=0;
	unsigned char y=0;
	char data[3];
	int xp,yp;
	int point;
	int semid;
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct aeminfo * __buf;
	}arg;

	int msgid;
	key_t msgkey;
	struct{
		long mtype;
		char mtext[256];
	}msgbuf;

	int st;
	struct sigaction sig={0};


	if((semid=semget(ftok("k14-6",'a'),1,0666|IPC_CREAT))== -1){//キー生成し、セマフォ集合を取得
		perror("semget");
		return 1;
	}
	arg.val=1;
	if(semctl(semid,0,SETVAL,arg)==-1){//0番目のセマフォに値1をセット
		perror("semctl");
		return 1;
	}

	// msgsendはこのプロセスのプログラムファイル名
	if((msgkey=ftok("k14-6",'a'))==-1){ //キー生成
		perror("ftok");
		return 1;
	}
	if((msgid=msgget(msgkey,0666 | IPC_CREAT))==-1){ //メッセージキューの生成・取得
		perror("msgget");
		return 1;
	}
	
	if((i1=fork())==0){//スイッチプロセス
		fd0=open(FIFO,O_WRONLY);
		fd1=open(DEVNAME0,O_RDONLY);
		while(1){
			read(fd1,&ev,sizeof(ev));
			//イベント終了
			switch(ev.type){			
				case EV_SYN:
					sprintf(msgbuf.mtext,"EV_SYN\n");
					break;
				//イベント発生
				case EV_KEY:
					sprintf(msgbuf.mtext,"EV_KEY code=%d value=%d\n",ev.code,ev.value);
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
						sem_lock(semid); //共有リソース（FIFO）に書き込む前に、ロックをかける
						write(fd0,&point,sizeof(point));
						sem_unlock(semid); //共有リソース（FIFO）を使用後に、アンロックする
					}
					break;
			}
			msgbuf.mtype=1;
			if(msgsnd(msgid,&msgbuf,256,0)==-1){//メッセージの送信
				perror("msgsnd");
				return 1;
			}		
		}
		close(fd0);
		close(fd1);
		_exit(1);
	}
	
	if((i2=fork())==0){//タッチパネルプロセス
		fd0=open(FIFO,O_WRONLY);
		fd1=open(DEVNAME1,O_RDONLY);
		while(1){
			read(fd1,&ev,sizeof(ev));
			//イベント終了
			switch(ev.type){			
				case EV_SYN:
					sprintf(msgbuf.mtext,"EV_SYN\n");
					break;
				//イベント発生
				case EV_KEY:
					sprintf(msgbuf.mtext,"EV_KEY code=%d value=%d\n",ev.code,ev.value);
					if(ev.value==1)
					{
						sem_lock(semid); //共有リソース（FIFO）に書き込む前に、ロックをかける
						write(fd0,&point,sizeof(point));
						sem_unlock(semid); //共有リソース（FIFO）を使用後に、アンロックする
					}
					break;
				case EV_ABS:
					sprintf(msgbuf.mtext,"EV_ABS code=%d value=%d\n",ev.code,ev.value);
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
			msgbuf.mtype=1;
			if(msgsnd(msgid,&msgbuf,256,0)==-1){//メッセージの送信
				perror("msgsnd");
				return 1;
			}
		}
		close(fd0);
		close(fd1);
		_exit(1);
	}
	if((i3=fork())==0){//LEDプロセス
		fd0=open(FIFO,O_RDONLY);
		while(1){
			read(fd0,&point,sizeof(point));
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
			if((fd1=open(devfile,O_WRONLY))==-1){ //デバイスファイルを開く
				sprintf(buf,"%s:open():%s",argv[0],devfile);
				perror(buf);
				return 1;
			}
			if(write(fd1,data,1)==-1){  //LED_の制御
				sprintf(buf,"%s:write():%s",argv[0],devfile);
				perror(buf);
				return 1;
			}
			close(fd1);
		}
		close(fd0);
		_exit(1);
	}
	if((i4=fork())==0){//LEDプロセス
		while(1){
			if(msgrcv(msgid,&msgbuf,256,1,0)==-1){//メッセージの受信
				 perror("msgrcv");
				 return 1;
			}
			printf("%s",msgbuf.mtext);
			fd0=open(LOG,O_WRONLY | O_APPEND | O_CREAT);
			write(fd0,msgbuf.mtext,strlen(msgbuf.mtext));
			close(fd0);
		}
	}
	printf("%s PID=%d %d %d %d\n",argv[0],i1,i2,i3,i4);
	sig.sa_handler= killchild;//ハンドラ関数を設定
	sigaction(SIGINT,&sig,NULL);//シグナルINTを受けたら、関数killchaildを呼び出す。
	wait(&st);
	kill(i1,15);
	kill(i2,15);
	kill(i3,15);
	kill(i4,15);
	return 0;
}

int sem_lock(int semid)
{
	struct sembuf sb;
	sb.sem_num=0; //操作するセマフォの番号
	sb.sem_op=-1; //ロック操作は負の値
	sb.sem_flg=0;
	if(semop(semid,&sb,1)== -1){ //0番目のセマフォの値を-1する
		perror("semop");
		return -1;
	}
	return 1;
}

int sem_unlock(int semid)
{
	struct sembuf sb;
	sb.sem_num=0; //操作するセマフォの番号
	sb.sem_op=1; //アンロック操作は正の値
	sb.sem_flg=0;
	if(semop(semid,&sb,1)== -1){ //0番目のセマフォの値を+1する
		perror("semop");
		return -1;
	}
	return 1;
}

void killchild(int signum)
{
	kill(i1,15);
	kill(i2,15);
	kill(i3,15);
	kill(i4,15);
	_exit(1);
}



