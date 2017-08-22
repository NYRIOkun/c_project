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
    int i;
    int fd;
    int file_read;
    char buf[256];
    char data[1025];
    
    printf("argc : %d\n",argc);
    for(i=0;i<argc;i++){
        printf("argv[%d] : %s\n",i,argv[i]);
    }
    if((fd=open(argv[1],O_RDONLY))==-1){
        perror("open()");
        return 1;
    }
    printf("fd : %d\n",fd);
    (file_read=read(fd,data,sizeof(data)));
    //data[file_read]='\0';
    printf("data : \n%s\nsizeof(data) : %ld\n",data,sizeof(data));
    printf("file_read : %d\n",file_read);
    
    close(fd);
    return 0;
}