#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc,char *argv[])
{
    int fd;
    int nwrite;
    char buf[256];
    char data[1025];
    
    if(argc == 1){
        printf("usage: %s <filename> \n",argv[0]);
        return 1;
    }
    if((fd=open(argv[1],O_WRONLY | O_CREAT | O_EXCL,00711)) == -1){
        sprintf(buf,"%s:open(): %s",argv[0],argv[1]);
        perror(buf);
        return 1;
    }
    
    while(fgets(data,1024,stdin) != NULL)
    {
        nwrite=strlen(data);
        if(write(fd,data,nwrite)==-1){
            sprintf(buf,"%s:write:%s",argv[0],argv[1]);
            perror(buf);
            return 1;
        }
    }
    close(fd);
    return 0;
}