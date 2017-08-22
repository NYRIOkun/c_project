#include <unistd.h> //for fork()
#include <stdio.h>
#include <time.h>
#include <fcntl.h>

int main(void)
{
    int fd,i;
    char buff[256];
    time_t now;
    buff[0]='\0';
    
    //書き込みONLY
    if(fork()==0){
        if((fd=open("testfifo",O_WRONLY)) != -1){
            for(i=0;i<20;i++){
                now=time(NULL);
                write(fd,ctime(&now),256);
                sleep(1);
            }
        }
        close(fd);
        _exit(0);
    }
    //読み込みONLY
    if(fork()==0){
        if((fd=open("testfifo",O_RDONLY)) != -1){
            for(i=0;i<20;i++){
                read(fd,buff,256);
                printf("%s\n",buff);
            }
        }
        close(fd);
        _exit(0);
    }
    return 0;
}
