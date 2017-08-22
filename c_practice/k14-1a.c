#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
	int i1,i2,i3,st;
	
	if((i1=fork())==0){//スイッチプロセス
		if(execl("k14-1a1","k14-1a1",NULL)==-1)
			_exit(1);
	}
	if((i2=fork())==0){//タッチパネルプロセス
		if(execl("k14-1a2","k14-1a2",NULL)==-1)
			_exit(2);
	}
	if((i3=fork())==0){//LED点灯プロセス
		if(execl("k14-1a3","k14-1a3",NULL)==-1)
			_exit(3);
	}
	printf("%s PID=%d %d %d\n",argv[0],i1,i2,i3);
	wait(&st);
	kill(i1,15);
	kill(i2,15);
	kill(i3,15);
	return 0;
}

