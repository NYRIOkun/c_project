/*****************
 * this source only use in armadillo460 with linux
 * never use in your workspace
 ****************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

//LEDファイルをあらかじめdefine
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
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
    char buf[256];
    char devfile[40];
    
    if(argc != 3){
        printf("usage: ledtest <rlgly> <on|off>\n");
        return 0;
    }
    switch(argv[1][0]){     //1行目の1文字目を取得してスイッチ
        case 'r':
            strcpy(devfile,R_LED);
            break;
        case 'g':
            strcpy(devfile,G_LED);
            break;
        case 'y':
            strcpy(devfile,Y_LED);
            break;
    }
    if((fd=open(devfile,O_WRONLY)) == -1){ //デバイスファイルを開く
        sprintf(buf,"%s:open(): %s",argv[0],devfile);
        perror(buf);
        return 1;
    }
    if(strcmp(argv[2],"on")==0){
        if(write(fd,"1",1)==-1){  //LED_ON
            sprintf(buf,"%s:write():%s",argv[0],devfile);
            perror(buf);
            return 1;
        }
    }else{  //LED_OFF
        if(write(fd,"0",1)==-1){
            sprintf(buf,"%s:write():%s",argv[0],devfile);
            perror(buf);
            return 1;
        }
    }
    close(fd);
    return 0;
}