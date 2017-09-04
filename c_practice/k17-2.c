#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>

#include <sqlite3.h>

#include <time.h>

#define SIZE 20
// DBファイル名
const char *fileName = "shoumei2.db";


//tb_ev用の構造体定義
typedef struct {
	int ev_id;
	char ev_tm[SIZE]; // yyyy/mm/dd hh:mm:ss\0 20bytes
	int ev_op;
	int rm_id;
}INS_DATA_EV;

#define DEVNAME0 "/dev/input/event0"
#define DEVNAME1 "/dev/input/event1"
#define R_LED "/sys/class/leds/red/brightness"
#define G_LED "/sys/class/leds/green/brightness"
#define Y_LED "/sys/class/leds/yellow/brightness"
#define FIFO "testfifo"

void get_now_time(char *buf,int n);
int set_data(int room_id,int event_op);

int main(int argc,char *argv[])
{
	int i1,i2,i3,st;
	int fd0,fd1;
	struct input_event ev;
	char buf[256];
	char devfile[40];
	unsigned char r=0;
	unsigned char g=0;
	unsigned char y=0;
	char data[3];
	int xp,yp;
	int point;

	if((i1=fork())==0){//スイッチプロセス
		fd0=open(FIFO,O_WRONLY);
		fd1=open(DEVNAME0,O_RDONLY);
		while(1){
			read(fd1,&ev,sizeof(ev));
			//イベント終了
			switch(ev.type){
				case EV_SYN:
					//printf("EV_SYN\n");
					break;
				//イベント発生
				case EV_KEY:
					//printf("EV_KEY code=%d value=%d\n",ev.code,ev.value);
					if(ev.value==1)
					{
						switch(ev.code){
							case 158:
								point=1;
								break;
							case 139:
								point=2;
								break;
							case 102:
								point=3;
								break;
							default:
								point=0;
						}
						write(fd0,&point,sizeof(point));
					}
					break;
			}
		}
		close(fd0);
		close(fd1);
		_exit(1);
	}

	if((i2=fork())==0){//タッチパネルプロセス
		fd0=open(FIFO,O_WRONLY);
		fd1=open(DEVNAME1,O_RDONLY);
		while(1){
			read(fd1,&ev,sizeof(ev));
			//イベント終了
			switch(ev.type){
				case EV_SYN:
					//printf("EV_SYN\n");
					break;
				//イベント発生
				case EV_KEY:
					//printf("EV_KEY code=%d value=%d\n",ev.code,ev.value);
					if(ev.value==1)
					{
						write(fd0,&point,sizeof(point));
					}
					break;
				case EV_ABS:
					//printf("EV_ABS code=%d value=%d\n",ev.code,ev.value);
					if(ev.code==0) //X座標(横方向）の時
						xp=ev.value;
					if(ev.code==1) //Y座標(縦方向）の時
						yp=ev.value;
					if(xp<1400)
						point=3;
					else if(xp<2700)
						point=2;
					else
						point=1;
					break;
			}
		}
		close(fd0);
		close(fd1);
		_exit(1);
	}
	if((i3=fork())==0){//LEDプロセス
		fd0=open(FIFO,O_RDONLY);
		while(1){
			read(fd0,&point,sizeof(point));
			switch(point){
				case 1:
					strcpy(devfile,R_LED);
					r=r^1; //0->1  1->0 にする
					set_data(point,r); //データベースに登録
					sprintf(data,"%d",r);
					break;
				case 2:
					strcpy(devfile,G_LED);
					g=g^1;
					set_data(point,g); //データベースに登録
					sprintf(data,"%d",g);
					break;
				case 3:
					strcpy(devfile,Y_LED);
					y=y^1;
					set_data(point,y); //データベースに登録
					sprintf(data,"%d",y);
					break;
			}
			if((fd1=open(devfile,O_WRONLY))==-1){ //デバイスファイルを開く
				sprintf(buf,"%s:open():%s",argv[0],devfile);
				perror(buf);
				return 1;
			}
			if(write(fd1,data,1)==-1){  //LED_の制御
				sprintf(buf,"%s:write():%s",argv[0],devfile);
				perror(buf);
				return 1;
			}
			close(fd1);
		}
		close(fd0);
		_exit(1);
	}
	printf("%s PID=%d %d %d\n",argv[0],i1,i2,i3);
	wait(&st);
	kill(i1,15);
	kill(i2,15);
	kill(i3,15);
	return 0;
}


int set_data(int room_id,int event_op) {
	int err = 0;
	INS_DATA_EV ins_data_ev;

	get_now_time(ins_data_ev.ev_tm,SIZE);
	ins_data_ev.ev_op=event_op;
	ins_data_ev.rm_id=room_id;


	// データベースオブジェクト
	sqlite3 *pDB = NULL;
	// ステートメントオブジェクト
	sqlite3_stmt *pStmt = NULL;

	// データベースのオープン
	printf("データベースのオープン\n");
	err = sqlite3_open(fileName, &pDB);
	//エラー処理
	if(err != SQLITE_OK){
		printf("sqlite3_open Err! %d\n", err);
	}

	// イベントデータの追加
	printf("イベントデータの追加\n");
	// tb_lp用ステートメントの用意
	err = sqlite3_prepare_v2(pDB,"INSERT INTO tb_ev VALUES(?,?,?,?)",-1, &pStmt, NULL);

	if(err != SQLITE_OK){
		printf("sqlite3_prepare_v2  ERR! %d \n",err);
	}else{
		printf("sqlite3_prepare_v2 OK!\n");

		// ステートメントの?の部分に型変換して値を代入
		sqlite3_bind_null(pStmt, 1);
		sqlite3_bind_text(pStmt, 2, ins_data_ev.ev_tm,strlen(ins_data_ev.ev_tm), SQLITE_TRANSIENT);
		sqlite3_bind_int(pStmt, 3, ins_data_ev.ev_op);
		sqlite3_bind_int(pStmt, 4, ins_data_ev.rm_id);

		//SQLITE_BUSYの間は繰り返し
		while(SQLITE_BUSY ==(err= sqlite3_step(pStmt))){}
		printf("sqlite3_step　err=%d\n",err);
		//エラー処理（主キー重複エラー）
		if(err == SQLITE_CONSTRAINT){
			printf("%s\n",sqlite3_errmsg(pDB));
		}
		//ステートメントの初期化
		sqlite3_reset(pStmt);
	}

	// ステートメントの解放
	sqlite3_finalize(pStmt);

	// データベースのクローズ
	printf("データベースのクローズ\n");
	err = sqlite3_close(pDB);
	//エラー処理
	if(err != SQLITE_OK){
		printf("sqlite3_close ERR! %d\n", err);
	}

	return 0;
}

void get_now_time(char *buf,int n)
{
	struct tm now;
	time_t sec;

	sec=time(NULL); //time_t time(time_t *t);
	localtime_r(&sec, &now);
	snprintf(buf,n,"%04d/%02d/%02d %02d:%02d:%02d",
		now.tm_year + 1900, now.tm_mon +1,
		now.tm_mday, now.tm_hour,
		now.tm_min, now.tm_sec);
	return;
}
