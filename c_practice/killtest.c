#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <ctype.h>
#include <sys/types.h>

int main(void
){
    int fd1[2],fd2[2],st,i;
    pid_t pid1,pid2;
    char buf[1024];
    if(pipe(fd1)!=0){
        perror("pipe");
        exit(1);
    }
    if(pipe(fd2)!=0){
        perror("pipe");
        exit(1);
    }
    if(fork()==0){
        while(printf("input:?"),fgets(buf,1024,stdin)!=NULL){
            write(fd1[1],buf,1024);
            sleep(1);
        }
        return 0;
    }
    if((pid1=fork())==0){
        while(1){
            read(fd1[0],buf,1024);
            for(i=0;i<1024;i++){
                if(buf[i]=='\n' | buf[i]=='\0')break;
                buf[i]=toupper(buf[i]);
            }
            write(fd2[1],buf,1024);
        }
    }
     if((pid2=fork())==0){
        while(1){
            read(fd2[0],buf,1024);
            printf("%s",buf);
        }
     }
    wait(&st);
    kill(pid1,SIGTERM);
    kill(pid2,SIGTERM);
}