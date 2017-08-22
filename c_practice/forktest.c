#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>

int main(int argc,char *argv[])
{
    int tm,i,j;
    if(argc==1){
        fprintf(stderr,"usage : %s <Second>\n",argv[0]);
        return 1;
    }
    tm = atoi (argv[1]) ;
    if(tm <= 0){
        fprintf(stderr,"usage : %s Please appoint right time\n",argv[0]);
        return 1;
    }
    if((i = fork()) == 0){
        sleep(tm);
        fprintf(stderr,"%d:It is time",i);
    }else{
        fprintf(stderr,"%d:diplay it %d seconds later\n",i,tm);
        if((j = fork()) == 0){
            fprintf(stderr,"%d:diplay it %d seconds later\n",j,tm*2);
            sleep(tm *= 2);
        }
    }
    sleep(10);
    return 0;
}