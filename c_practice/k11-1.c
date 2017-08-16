#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

int main(int argc, char *argv[])
{
	int fd;
	char buf[256];



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
		sleep(1);
		if(write(fd,"0",1)==-1){  //LED_OFF
			perror("led_blink:write():");
			return 1;
		}
		sleep(1);
	}
	close(fd);
	return 0;
}
