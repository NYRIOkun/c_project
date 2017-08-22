#include <time.h>   //for ctime()
#include <stdio.h>  //for printf()
#include <unistd.h> //for fork()
#include <fcntl.h>  //for open(),close()
#include <string.h> //for strlen()
#include <linux/input.h> //structの中身はこの中にあるよ

/*********************************
 * ===============================
 * execl(*<path>,*<arg>,<>)
 * execlp(*<file>,*<arg>,<>)
 * ===============================
 * usage			: 他コマンドやプログラムを呼び出します
 * <path>	 		: コマンドのパスを指定します
 * <file>			: ファイルのパスを指定します
 * <arg>			: 入力したコマンドの内容
 * ==============================
 ********************************/

int main(int argc,char *argv[])
{
    int i;
    //インプット構造体
    struct input_event ev;
    
    //processの分裂
    if((i = fork()) == 0){
        //PID0(親)の処理
        if(execl("/root/k11-2","k11-2",NULL)==-1)_exit(1);
    }else if((i = fork()) == 0){
        //PID0以外(子)の処理
        if(execl("/root/k11-1","k11-1",NULL)==-1)_exit(1);
    }else{
        //PID0以外(子)の処理
        if(execl("/usr/bin/top","/usr/bin/top",NULL) == -1)_exit(1);
    }
    return 0;
}
