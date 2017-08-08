#include <time.h> //time関連の関数使用に役に立つ
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/input.h> //structの中身はこの中にあるよ


/*********************************
 * ===============================
 * read(<filename>,<text>,<byte>)
 * ===============================
 * usage        : open済みのfileを読み込む
 * <filename>   : 読み込むファイル名(open済み前提)
 * <text>       : 読み込む内容
 * <byte>       : <text>に必要なbyte
 * ==============================
 ********************************/

int main(int argc,char *argv[])
{
    int fd,fd_led;
    char flash=0;
    char buf[256];
    
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
        //switch ev.type_status
        if(ev.code==28) {
            /*led*/
            //LED file open
            if((fd_led=open("/sys/class/leds/red/brightness",O_WRONLY)) == -1){
                sprintf(buf,"%s:open()",argv[0],fd_led);
                perror(buf);
                close(fd_led);
                return 1;
            }
            //flash = turn on flag
            if(flash==0){
                if((write(fd_led,"1",1))==-1){
                    sprintf(buf,"%s:write():%s",argv[0],fd_led);
                    perror(buf);
                    close(fd_led);
                    return 1;
                }
            }else{ 
                if((write(fd_led,"0",1))==-1){
                    sprintf(buf,"%s:write():%s",argv[0],fd_led);
                    perror(buf);
                    close(fd_led);
                    return 1;
                }
            }
            flash = ~flash;
            close(fd_led);
        }
    }
    close(fd);
    return 0;
}