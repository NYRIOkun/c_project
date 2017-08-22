#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"
#define FIFO "testfifo"

int main(int argc,char *argv[])
{
	int fd0,fd1;
	char buf[256];
	char devfile[40];
	unsigned char r=0;
	unsigned char g=0;
	unsigned char y=0;
	char data[3];
	int point;
	
	fd0=open(FIFO,O_RDONLY);
	while(1){
		read(fd0,&point,sizeof(point));
		switch(point){
			case 1:
				strcpy(devfile,R_LED);
				r=r^1; //0->1  1->0 にする
				sprintf(data,"%d",r);
				break;
			case 2:
				strcpy(devfile,G_LED);
				g=g^1;
				sprintf(data,"%d",g);
				break;
			case 3:
				strcpy(devfile,Y_LED);
				y=y^1;
				sprintf(data,"%d",y);
				break;
		}	
		if((fd1=open(devfile,O_WRONLY))==-1){ //デバイスファイルを開く
			sprintf(buf,"%s:open():%s",argv[0],devfile);
			perror(buf);
			return 1;
		}
		if(write(fd1,data,1)==-1){  //LED_の制御
			sprintf(buf,"%s:write():%s",argv[0],devfile);
			perror(buf);
			return 1;
		}
		close(fd1);
	}
	close(fd0);
	return 0;
}
