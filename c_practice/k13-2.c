#include <time.h>   //for ctime()
#include <stdio.h>  //for printf()
#include <unistd.h> //for fork()
#include <stdlib.h> //for system()
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

int main(void)
{
    system("./k11-3 &");
    system("./k11-5 &");
    return 0;
}
