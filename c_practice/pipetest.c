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

int main(void)
{
    int fd[2],data,st;
    char buff[256];
    
    if(pipe(fd)!=0){
        perror("pipe");
        _exit(1);
    }
    if(fork()==0){
        read(fd[0],buff,256);
        printf("%s",buff);
        fgets(buff,256,stdin);
        write(fd[1],buff,256);
        _exit(0);
    }
    write(fd[1],"What's your name ?",256);
    wait(&st);
    read(fd[0],buff,256);
    printf("%s",buff);
    return 0;
}
