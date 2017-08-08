#include <time.h> //time関連の関数使用に役に立つ
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#define clear \x1b[2J
#define mv_cursor \x1b[0;0H

int main(void)
{
    int fd;
    time_t now;
    char buf[256];
    
    if((fd=open("/dev/tty1",O_WRONLY)) == -1){
        perror("lcd_time:open():");
        return 1;
    }
    strcpy(buf,"\x1b[2J");
    if(write(fd,buf,strlen(buf))==-1){
            perror("lcd_time:write():");
            return 1;
    }
    

    while(1){
        now = time(NULL);
        
        sprintf(buf,"\x1b[0;0H%s\n",ctime(&now));
        if(write(fd,buf,strlen(buf))==-1){
            perror("lcd_time:write():");
            return 1;
        }
        sleep(1);
    }
    close(fd);
    return 0;
}