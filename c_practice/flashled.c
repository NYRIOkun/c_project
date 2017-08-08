#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define Y_LED "/sys/class/leds/yellow/brightness"

/*********************************
 * ===============================
 * write(<filename>,<text>,<byte>)
 * ===============================
 * usage        : open済みのfileに文字列を書き込む
 * <filename>   : 書き込むファイル名をしてする(open済み前提)
 * <text>       : 書き込む内容
 * <byte>       : <text>に必要なbyte
 * ==============================
 ********************************/

int main(int argc,char *argv[])
{
    int fd;
    char flash_flg = 0;
    char buf[256];
    char devfile[40];
    
    if(argc != 1){
        printf("usage: flashled\n");
        return 0;
    }
    strcpy(devfile,Y_LED);
   
    if((fd=open(devfile,O_WRONLY)) == -1){ //デバイスファイルを開く
        sprintf(buf,"%s:open(): %s",argv[0],devfile);
        perror(buf);
        return 1;
    }
    while(1){
        if(flash_flg){
            if(write(fd,"1",1)==0){
                sprintf(buf,"%s:write(): %s",argv[0],devfile);
                perror(buf);
                return 1;
            };
        }else{
            if(write(fd,"0",1)==0){
                sprintf(buf,"%s:write(): %s",argv[0],devfile);
                perror(buf);
                return 1;
            }
        }
        flash_flg = ~flash_flg;
        sleep(1); //引数は秒(s)...msではない
    }
    close(fd);
    return 0;
}
