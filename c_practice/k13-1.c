#include <time.h>   //for ctime()
#include <stdio.h>  //for printf()
#include <unistd.h> //for fork()
#include <fcntl.h>  //for open(),close()
#include <string.h> //for strlen()
#include <linux/input.h> //structの中身はこの中にあるよ


#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"
#define TIMER "/dev/tty1"

int main(int argc,char *argv[])
{
    int fd_timer;
    int fd_led;
    int i;
    time_t now;
    char flash_flg = 0;
    char buf[256];

    //インプット構造体
    struct input_event ev;
    
    //processの分裂
    if((i = fork()) == 0){
        
        //PID0(親)の処理
        if((fd_timer = open(TIMER,O_WRONLY)) == -1){
            return 1;
        }

        while(1){
           now = time(NULL);
           //\x1b[0;0H%s\n = 画面クリアの制御文字
           sprintf(buf,"\x1b[0;0H%s\n",ctime(&now));
           if(write(fd_timer,buf,strlen(buf)) == -1){
               return 1;
           }
           sleep(1);
        }
    }else{
        
        //PID0以外(子)の処理
        if((fd_led = open(Y_LED,O_WRONLY)) == -1){
            return 1;
        }
        while(1){
            if(flash_flg){
                if(write(fd_led,"1",1) == 0){
                    return 1;
                };
            }else{
                if(write(fd_led,"0",1) == 0){
                    return 1;
                }
            }
            flash_flg = ~flash_flg;
            sleep(1); //引数は秒(s)...msではない
        }
    }
    close(fd_timer);
    close(fd_led);
    return 0;
}
