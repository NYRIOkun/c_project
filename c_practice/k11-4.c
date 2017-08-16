#include <time.h> //time関連の関数使用に役に立つ
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h> //structの中身はこの中にあるよ

int main(int argc,char *argv[])
{
    int fd,fd_led;
    char devfile[50];
    char buf[256];
    char R_flash=0,Y_flash=0,G_flash=0;
    char common_flash=0;
    
    //インプット構造体
    struct input_event ev;
    
    //event switch
    if((fd=open("/dev/input/event1",O_RDONLY)) == -1){
        sprintf(buf,"%s:open(): %s",argv[0],"/dev/input/event1");
        perror(buf);
        return 1;
    }
    while(1){
        //read key event
        if(read(fd,&ev,sizeof(ev))==-1){
            sprintf(buf,"%s:read()",argv[0]);
            perror(buf);
            close(fd);
            return 1;
        }
        printf("%d . %d ",ev.time.tv_sec,ev.time.tv_usec);
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
    close(fd);
    return 0;
}