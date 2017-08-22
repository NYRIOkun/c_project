#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
int main(int argc,char *argv[])
{
	int i,j,r;
	if(argc==1){
		fprintf(stderr,"Error\n");
		return 1;
	}
	srand(argv[1][0]);
	for(i=0;i<10;i++){
		r=rand()%4+1;
		printf("\n%c:",argv[1][0]);
		for(j=0;j<r;j++){
			printf("%d",r);
			fflush(stdout);
			sleep(1);
		}
		sleep(1);
	}
	printf("\nend:%s %s\n",argv[0],argv[1]);
	return 0;
}

