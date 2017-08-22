#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(int argc,char *argv[])
{
	int semid;
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo * __buf;
	}arg;
	
	if((semid=semget(ftok("semmk",'a'),1,0666 | IPC_CREAT)) == -1){
		perror("semget");
		return 1;
	}
	arg.val=1;
	if(semctl(semid,0,SETVAL,arg) == -1){
		perror("semctl");
		return 1;
	}
	printf("id=%d\nvalue=%d\n",semid,semctl(semid,0,GETVAL,arg));
	return 0;
}

