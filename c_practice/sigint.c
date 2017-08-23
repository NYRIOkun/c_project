#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void inthandler(int signum);

unsigned long count;

int main(void)
{
    struct sigaction sig={0};
    count=0;
    sig.sa_handler= inthandler;
    sigaction(SIGINT,&sig,NULL);
    while(count<1000000L){
        printf("Ahh!!");
        count++;
    }
    return 0;
}

void inthandler(int signum)
{
    printf("\nSIGINT=%d:%ld\n",signum,count);
    _exit(1);
}