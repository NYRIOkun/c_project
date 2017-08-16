#include <time.h> //time関連の関数使用に役に立つ
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h> //structの中身はこの中にあるよ
#include <sys/select.h> //structの中身はこの中にあるよ

#define DEVNAME0 "/dev/input/event0"
#define DEVNAME1 "/dev/input/event1"

int main(int argc,char *argv[])
{
    int fd0,fd1;
    struct input_event ev;
    fd_set readfs;
    int maxfd;
    
    fd0=open(DEVNAME0,O_RDONLY);
    fd1=open(DEVNAME1,O_RDONLY);
    maxfd=(fd1>fd0)? fd1+1:fd0+1;
    
    while(1){
        FD_SET(fd0,&readfs);
        FD_SET(fd1,&readfs);
        select(maxfd,&readfs,NULL,NULL,NULL);
        if(FD_ISSET(fd0,&readfs)){
            read(fd0,&ev,sizeof(ev));
        }
        if(FD_ISSET(fd1,&readfs)){
            read(fd1,&ev,sizeof(ev));
        }
        printf("%d : %d",ev.time.tv_sec,ev.time.tv_usec);
        switch(ev.type){
            case EV_SYN:
                printf("EV_SYN\n");
                break;
            case EV_KEY:
                printf("EV_KEY code= %d. value = %d \n",ev.code,ev.value);
                break;
            case EV_ABS:
                printf("EV_ABS code= %d. value = %d \n",ev.code,ev.value);
                break;
        }
    }
    close(fd0);
    close(fd1);
    return 0;
}