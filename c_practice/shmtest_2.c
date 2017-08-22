/* +α　2つのファイルに分ける課題　*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

int main(void)
{
	key_t shmkey;
	int shmid;
	char data[4096];
	char * pos;

	shmkey=ftok("shmtest",'a'); //kye生成  shmemはこのプロセスのプログラムファイル名
	shmid=shmget(shmkey,sizeof(data),IPC_CREAT | 0666); //共有メモリの生成・取得
	pos=shmat(shmid,0,0);  //共有メモリにアタッチ
	sleep(1);	
	printf("2:%s\n",pos);
	strcat(pos," receve");
	shmdt(pos); //共有メモリをデタッチ
	return 0;
}
