#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>


void tmh(int signum);

int main(void)
{
    struct sigaction sig={0};
    struct itimerval val;
    time_t now;
    sig.sa_handler=tmh;
    sigaction(SIGALRM,&sig,NULL);
    val.it_interval.tv_sec=val.it_value.tv_sec=0;
    val.it_interval.tv_usec=val.it_value.tv_usec=500000;
    setitimer(ITIMER_REAL,&val,NULL);
    while(1){
        pause();
        now=time(NULL);
        printf("%s",ctime(&now));
    }
	return 0;
}

void tmh(int signum)
{
    return;
}
