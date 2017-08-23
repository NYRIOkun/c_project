#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h>

#define RIGHT_SW    158
#define CENTER_SW   139
#define LEFT_SW     102
#define R_LED       "/sys/class/leds/red/brightness"
#define G_LED       "/sys/class/leds/green/brightness"
#define Y_LED       "/sys/class/leds/yellow/brightness"
#define SWFILE      "/dev/input/event0"


int LED(char color);
/*********************************
 * ===============================
 * void LED(<color>)
 * ===============================
 * usage        : LEDの色を操作するよ
 * <color>      : r,g,yのいづれかを入れてね
 * ==============================
 ********************************/

int main(int argc,char *argv[])
{
    int i;
    int test_fd;
    int sw_fd;
    int file_read;
    char buf[256];
    char data[1025];
    
    struct input_event ev;
    
    printf("argc : %d\n",argc);
    for(i=0;i<argc;i++){
        printf("argv[%d] : %s\n",i,argv[i]);
    }
    
    //ファイルのオープン
    if(((test_fd=open(argv[1],O_RDONLY))==-1)
        || ((sw_fd=open(SWFILE,O_RDONLY))==-1))
    {
        perror("open()");
        return 1;
    }
    
    printf("test_fd : %d\n",test_fd);   //for showcase
    printf("sw_fd : %d\n",sw_fd);       //for switch-status
    
    //ファイルの読み込み
    if(file_read=read(test_fd,data,sizeof(data)-1)==-1){
        perror("read()");
        return 1;
    }
    //data[file_read]='\0';
    printf("data : \n%s\nsizeof(data) : %ld\n",data,sizeof(data));
    printf("file_read : %d\n",file_read);
    
    while(1){
        if(read(sw_fd,&ev,sizeof(ev))==-1){
            perror("read()");
            return 1;
        }
        switch(ev.type){
            case EV_SYN:
                break;
            case EV_KEY:
                printf("ev.code : %d\nev.value : %d\n\n",ev.code,ev.value);
                if(ev.value==1){
                    switch(ev.code){
                        case LEFT_SW:
                            if(LED('r'))return 1;
                            break;
                        case CENTER_SW:
                            if(LED('g'))return 1;
                            break;
                        case RIGHT_SW:
                            if(LED('y'))return 1;
                            break;
                        default:
                            break;
                    }
                }
                break;
        }
    }
    
    close(test_fd);
    close(sw_fd);
    return 0;
}


int LED(char color)
{
    int led_fd;
    char devfile[40];
    static char r_statu=0;
    static char g_statu=0;
    static char y_statu=0;
    char color_status;
    
    switch(color){     //引数を取得してスイッチ制御
        case 'r':
            strcpy(devfile,R_LED);
            color_status=r_statu=r_statu^1;
            break;
        case 'g':
            strcpy(devfile,G_LED);
            color_status=g_statu=g_statu^1;
            break;
        case 'y':
            strcpy(devfile,Y_LED);
            color_status=y_statu=y_statu^1;
            break;
        default:
            perror("なんだその引数？！");
            return 1;
            break;
    }
    led_fd=open(devfile,O_WRONLY); //デバイスファイルを開く
    if(color_status == 1){
        write(led_fd,"1",1);
    }else{
        write(led_fd,"0",1);
    }
    close(led_fd);
    return 0;
}