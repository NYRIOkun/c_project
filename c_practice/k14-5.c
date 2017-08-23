#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>


#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

void tmh(int signum);

int main(int argc, char *argv[])
{
	int fd;
	char buf[256];
	struct sigaction sig={0};
    struct itimerval val;
    time_t now;
    sig.sa_handler=tmh;
    sigaction(SIGALRM,&sig,NULL);
    val.it_interval.tv_sec=val.it_value.tv_sec=0;
    val.it_interval.tv_usec=val.it_value.tv_usec=100000;
    setitimer(ITIMER_REAL,&val,NULL);


	if((fd=open(Y_LED,O_WRONLY))==-1){
		sprintf(buf,"%s:open():%s",argv[0],Y_LED);
		perror(buf);
		return 1;
	}
	while (1){
		if(write(fd,"1",1)==-1){  //LED_ON
			sprintf(buf,"%s:write():%s",argv[0],Y_LED);
			perror(buf);
			return 1;
		}
		pause();
		if(write(fd,"0",1)==-1){  //LED_OFF
			perror("led_blink:write():");
			return 1;
		}
		pause();
	}
	close(fd);
	return 0;
}

void tmh(int signum)
{
    return;
}
