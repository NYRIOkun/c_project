#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
        perror("msgget");
        return 1;
    }

    //msg2の受信
    if(msgrcv(msgID,&buf,256,2,0) == -1){
        perror("msgrcv");
        return 1;
    }
    printf("%s\n",buf.mText);

    //msg1の受信
    if(msgrcv(msgID,&buf,256,1,0) == -1){
        perror("msgrcv");
        return 1;
    }
    printf("%s\n",buf.mText);
    
    //msgの削除
    if(msgctl(msgID,IPC_RMID,NULL) == -1){
        perror("msgctl");
        return 1;
    }
    return 0;
}