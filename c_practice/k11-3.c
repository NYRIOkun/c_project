#include <time.h> //time関連の関数使用に役に立つ
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h> //structの中身はこの中にあるよ

#define LEFT 102
#define CENTER 139
#define RIGHT 158
#define HIDE 28
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"

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
    if((fd=open("/dev/input/event0",O_RDONLY)) == -1){
        sprintf(buf,"%s:open(): %s",argv[0],"/dev/input/event0");
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
        //if pushed button
        if(ev.value == 1){
            printf("pushed button is ");
            switch(ev.code){
                case LEFT:
                    printf("left\n");
                    strcpy(devfile,R_LED);
                    common_flash = R_flash = ~R_flash;
                    break;
                case CENTER:
                    printf("center\n");
                    strcpy(devfile,G_LED);
                    common_flash = G_flash = ~G_flash;
                    break;
                case RIGHT:
                    printf("right\n");
                    strcpy(devfile,Y_LED);
                    common_flash = Y_flash = ~Y_flash;
                    break;
                case HID:
                    printf("hide\n");
                    break;
                default:
                    break;
            };
            
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
    }
    close(fd);
    return 0;
}

