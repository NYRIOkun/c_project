#include <stdio.h>
#include <string.h>			//for strlen()
#include <time.h>
#include <fcntl.h>			//for open()
#include <linux/input.h>
#include <sys/select.h>
#include <sys/stat.h>		//for stat()
#include <sys/mman.h>		//for mmap(),munmap()

//prottype
int error_func(char *buf,char *text);

//origin define
#define error

//must define
#define DEVNAME0 "/dev/input/event0"
#define DEVNAME1 "/dev/input/event1"
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"


//get line of argc
int main(int argc,char *argv[])
{
	char *data;
	char buf[256];
	int fd;
	struct stat statbuf;
	//show Usage
	if(argc==1){
		printf("Usage: %s <filename> \n",argv[0]);
		return error;
	}
	
/*********************************
 * ===============================
 * stat(*<target>,*<file_pointer>)
 * ===============================
 * usage			: fileの情報を読み込んで,targetに格納する
 * <target> 		: 格納先　ポインタで宣言
 * <file_pointer>	: 読み込むデータ ポインタで宣言
 * return			: success:1、error:0
 * ==============================
 ********************************/
	if(stat(argv[1],&statbuf) != 0){
		sprintf(buf,"%s:stat()",argv[0]);
		perror(buf);
		return error;
	}
	printf("filesize %d Bytes\n\n",statbuf.st_size);
	if((fd=open(argv[1],O_RDONLY))<0){
		sprintf(buf,"%s:open():%s",argv[0],argv[1]);
		perror(buf);
		return error;
	}
/*********************************
 * ===============================
 * mmap(*<start>,<length>,<prot>,<flags>,<fd>,<offset>)
 * ===============================
 * usage			: fileの情報をメモリにマッピングする
 * <start>	 		: 始まりのメモリアドレス(NULLでシステム指定)
 * <length>			: アドレスの長さ指定
 * <fd>				: 格納先のファイル
 * <offset>			: 終端アドレス直後の、NULLメモリ数の設定
 * return			: success:1、error:0
 * other			: おまじない
 * ==============================
 ********************************/
	
	if((int)(data = mmap(NULL,statbuf.st_size,
						 PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0))==-1){
		sprintf(buf,"%s:mmap()",argv[0]);
		perror(buf);
		return error;
	}
	printf("%s\n",data);
	munmap(data,statbuf.st_size);
	close(fd);
	return 0;
	
}

int error_func(char *buf,char *text)
{
	sprintf(buf,"error function :%s()",text);
	perror(buf);
	return 1;
}