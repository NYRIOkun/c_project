#include <time.h> //time関連の関数使用に役に立つ
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h> //structの中身はこの中にあるよ

#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

int main(int argc,char *argv[])
{
    int fd,fd_led;
    char devfile[50];
    char buf[256];
    char flash=0,R_flash=0,Y_flash=0,G_flash=0;
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
        printf(" %d . %d ",ev.time.tv_sec,ev.time.tv_usec);
        switch(ev.type){
            case EV_SYN:
                printf("%d\n",ev.type);
                break;
            case EV_KEY:
                printf("type=%d code= %d. value = %d \n",ev.type,ev.code,ev.value);
                if(ev.value==1){
                    printf("copied!!\n");
                    common_flash = flash = ~flash;
                }
                break;
            case EV_ABS:
                printf("type=%d code= %d. value = %d \n",ev.type,ev.code,ev.value);
                if(ev.code==0){
                    if(ev.value > 2666){
                        strcpy(devfile,R_LED);
                    }else if(ev.value > 1333){
                        strcpy(devfile,Y_LED);
                    }else if(ev.value > 100){
                        strcpy(devfile,G_LED);
                    }
                }
                break;
            default:
                break;
        }
        
        if((fd_led = open(devfile,O_WRONLY)) == -1){
                sprintf(buf,"%s:open(): %s",argv[0],devfile);
                perror(buf);
                return 1;
        }
        if(!common_flash){
            if(write(fd_led,"1",1)== -1){
                sprintf(buf,"%s:write():%s",argv[0],devfile);
                perror(buf);
                return 1;
            }
        }else{
            if(write(fd_led,"0",1)== -1){
                    sprintf(buf,"%s:write():%s",argv[0],devfile);
                    perror(buf);
                    return 1;
            } 
        }
        close(fd_led);
    }
    close(fd);
    return 0;
}