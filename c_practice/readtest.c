#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/*********************************
 * ===============================
 * read(<filename>,<text>,<byte>)
 * ===============================
 * usage        : open済みのfileを読み込む
 * <filename>   : 読み込むファイル名(open済み前提)
 * <text>       : 読み込む内容
 * <byte>       : <text>に必要なbyte
 * ==============================
 * by debian system call
 ********************************/

int main(int argc,char *argv[])
{
    int fd;
    int nread;
    char buf[256];
    char data[1025];
    
    //usage message
    if(argc == 1){
        printf("usage: %s <filename>\n",argv[0]);
        return 1;
    }
    
    //file open
    if((fd=open(argv[1],O_RDONLY)) == -1){
        sprintf(buf,"%s:open(): %s",argv[0],argv[1]);
        perror(buf);
        return 1;
    }
    while((nread=read(fd,data,sizeof(data)-1)) != 0){
        if(nread == -1){
            sprintf(buf,"%s:read(): %s",argv[0],argv[1]);
            perror(buf);
            close(fd);
            return 1;
        }
        data[nread]='\0';//null-stringの追加
        printf("%s",data);
    }
    close(fd);
    return 0;
}
