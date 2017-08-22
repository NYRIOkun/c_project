#include <time.h>   //for ctime()
#include <stdio.h>  //for printf()
#include <unistd.h> //for fork()
#include <fcntl.h>  //for open(),close()
#include <string.h> //for strlen()
#include <linux/input.h> //structの中身はこの中にあるよ
#include <sys/wait.h>//for wait()

#define SIZE 256
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"
#define TIMER "/dev/tty1"

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
    execlp("ls","ls",NULL);
    return 0;
}
