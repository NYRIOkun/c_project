#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

int sem_lock(int semid);
int sem_unlock(int semid);

int main(int argc,char *argv[])
{
	int semid,i,j,r;
	if(argc==1){
		fprintf(stderr,"Error\n");
		return 1;
	}
	if((semid=semget(ftok("semmk",'a'),1,0666 | IPC_CREAT)) == -1){
		perror("semget");
		return 1;
	}
	
	srand(argv[1][0]);
	for(i=0;i<10;i++){
		r=rand()%4+1;
		sem_lock(semid);
		printf("\n%c:",argv[1][0]);
		for(j=0;j<r;j++){
			printf("%d",r);
			fflush(stdout);
			sleep(1);
		}
		sem_unlock(semid);
		sleep(1);
	}
	printf("\nend:%s %s\n",argv[0],argv[1]);
	return 0;
}

int sem_lock(int semid)
{
	struct sembuf sb;
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=0;
	if(semop(semid,&sb,1) == -1){
		perror("semop");
		return -1;
	}
	return 1;
}

int sem_unlock(int semid)
{
	struct sembuf sb;
	sb.sem_num=0;
	sb.sem_op=1;
	sb.sem_flg=0;
	if(semop(semid,&sb,1) == -1){
		perror("semop");
		return -1;
	}
	return 1;
}