#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
sigjmp_buf jpenv;

void tmh(int signum);

int main(void)
{
    struct sigaction sig={0};
    char buff[256],fn[256];
    sig.sa_handler= tmh;
    sigaction(SIGALRM,&sig,NULL);
    alarm(5);
    sigsetjmp(jpenv,1);
    printf("Filename ? ");
    scanf("%s",fn);
    snprintf(buff,256,"cat %s",fn);
    system(buff);
    return 0;
}

void tmh(int signum)
{
    char arg[]="/bin/ls";
    int st;
    printf("\n");
    if(fork()==0){
        execl(arg,arg,NULL);
    }
    wait(&st);
    siglongjmp(jpenv,0);
    _exit(1);
}