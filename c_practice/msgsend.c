#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

int main(void)
{
    int msgID;
    key_t msgKEY;
    struct{
        long mType;
        char mText[256];
    }buf;
    
    /*msgsend はこのプログラム名(回帰)*/
    //keyの作成
    if((msgKEY=ftok("msgSend",'a')) == -1){
        perror("ftok");
        return 1;
    }
    //queの生成と取得
    if((msgID=msgget(msgKEY,0666 | IPC_CREAT)) == -1){
        perror("ftok");
        return 1;
    }
    //1つ目のmsgの格納
    strncpy(buf.mText,"Hello!",256);
    buf.mType=1;
    
    //msgの送信
    if(msgsnd(msgID,&buf,256,0) == -1){
        perror("msgsnd");
        return 1;
    }
    //2つ目のmsgの格納
    strncpy(buf.mText,"Bye Bye!",256);
    buf.mType=2;
    
    //msgの送信
    if(msgsnd(msgID,&buf,256,0) == -1){
        perror("msgsnd");
        return 1;
    }
    return 0;
}