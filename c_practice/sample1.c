#include <kernel.h>
#include <itron.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "target_tsc.h"
#include "target_lcd.h"
#include "target_embdio.h"
#include "kernel_cfg.h"
#include "sample1.h"
#include "bitmap.h"

//プロトタイプ宣言
void jump(void);
void move(void);
void mario_anime(void);
void foot_line(UINT,UINT,UINT);
void coin(UINT,UINT,UINT);
void display_object(void);
void display_message(void);
BOOL touch(UINT,UINT,UINT,UINT);
void enemy_touch(UINT);
void led_operation(void);


//共通変数宣言
UW sw;							//sw
ER er;							//er
UINT s1,s2,s3,s4;				//sw割り当て
UINT mario_x=0,mario_y=0;		//現在のマリオの位置
UINT mario_x_re=0,mario_y_re=0;	//直前のマリオの位置
UINT jump_counter=0;			//ジャンプ中のカウンタ
BOOL mario_direction=1;         //マリオの向きフラグ
BOOL fall_down=0;				//落下中フラグ
BOOL jumping=0;					//jump中フラグ
BOOL led_on=0;					//LED表示フラグ
UINT led_count;					//coin取得数フラグ
UINT gameover_count;			//gameoverフラグ
BOOL title_message=0;			//titleのメッセージフラグ

//共通配列宣言
BOOL touched[]={0,0,0,0,0,0,0,0}; 						//coin表示フラグ配列
UINT coin_x[]={ 50,100,150,200,250,300,350,430};		//coin x軸(最大470)
UINT coin_y[]={250,150, 40,200,250, 20, 80,260};		//coin y軸(最大270)

UINT enemy_x[] = {400,  0};								//敵の位置配列
UINT enemy_y[] = {200,180};
UINT enemy_x_re[] = {400,  0};
UINT enemy_y_re[] = {200,180};

//タイトル画面タスク
void title_task(VP_INT exinf)
{
	UW x=0,y=0;//tscフラグ
	if(!title_message){
		lcd_bitmap(180,100,title,LCD_WHITE);
		embdio_lcd_puts(true,0,0,"game title");
	}else{
		lcd_bitmap(180,130,gameover_message,LCD_WHITE);
		embdio_lcd_puts(true,0,0,"game over!");
	}

	while(1){
		er=tsc_wait(&x,&y,TMO_FEVR);
		if(er==E_OK && x && y){						//何かしらタッチされたら開始
			syslog(LOG_INFO,"x=%d y=%d",x,y);
			lcd_clr(LCD_WHITE);
			wup_tsk(MAIN_TASK);
			ext_tsk();
		}
	}
	ext_tsk();
}

//7seg timerタスク
void timer_task(VP_INT exinf)
{
	UINT time=24;

	while(1){
		embdio_7seg(2,true,time/10,false);
		embdio_7seg(1,true,time%10,false);
		dly_tsk(1000);
		time--;
		if(time==0){				//TIME0でリセットタスク起動
			time=24;
			act_tsk(RESET);
		}
	}
	ext_tsk();
}

//resetタスク
void reset_task(VP_INT exinf)
{
	volatile UINT i;
	mario_x=0,mario_y=0;		//現在のマリオの位置
	mario_x_re=0,mario_y_re=0;	//直前のマリオの位置
	jump_counter=0;				//ジャンプ中のカウンタ
	mario_direction=1;       	//マリオの向きフラグ
	fall_down=0;				//落下中フラグ
	jumping=0;					//jump中フラグ
	led_on=0;					//LED表示フラグ
	led_count=0;				//coin取得数フラグ
	enemy_x[0]=400;				//敵の初期位置
	enemy_y[0]=0;
	enemy_x[1]=200;
	enemy_y[1]=180;

	for(i=0;i<8;i++){
		touched[i]=0;			//coin取得フラグリセット
		embdio_led(3+i,false);
	}
	lcd_clr(LCD_WHITE);			//全画面のクリア
	gameover_count--;
	if(gameover_count==0){		//game_overフラグを増やす
		title_message=1;
		act_tsk(GAMEOVER);
	}
	ext_tsk();
}

//enemyタスク
void enemy_task(VP_INT exinf)
{
	BOOL flag=0;
	while(1){
		if(enemy_x[exinf]==2)flag=1;	//右端で左転換
		if(enemy_x[exinf]==460)flag=0;	//左端で右転換
		enemy_x[exinf] +=(!flag? -1:1);	//flagに応じて移動
		dly_tsk(2*(exinf+1));

		//enemyの表示処理
		if(enemy_x[exinf] != enemy_x_re[exinf]){
			lcd_box(enemy_x_re[exinf],enemy_y_re[exinf],
					enemy_x_re[exinf]+18,enemy_y_re[exinf]+17,LCD_WHITE,true);
		}
		lcd_bitmap(enemy_x[exinf],enemy_y[exinf],poison,LCD_WHITE);
		enemy_x_re[exinf] = enemy_x[exinf];									//現在の状態記録
		enemy_y_re[exinf] = enemy_y[exinf];
	}
	ext_tsk();
}

//game overタスク
void gameover_task(VP_INT exinf)
{
  ter_tsk(MAIN_TASK);
  ter_tsk(ENEMY);
  ter_tsk(ENEMY2);
  ter_tsk(TIME);
  act_tsk(MAIN_TASK);
  ext_tsk();
}

//main処理
void main_task(VP_INT exinf)
{
	syslog_msk_log(LOG_UPTO(LOG_INFO),LOG_UPTO(LOG_EMERG));
	syslog(LOG_INFO,"start program");
	syslog(LOG_INFO,"デバッグしたい場合は、このメッセージを検証箇所へ挿入してください");

	act_tsk(RESET);					//初期化実施
	embdio_7seg(2,false,0,false);	//7seg消灯
	embdio_7seg(1,false,0,false);
	act_tsk(TITLE);					//タイトル画面へ移行
	slp_tsk();						//起動されるまでスリープ
	act_tsk(TIME);					//タイマー起動
	act_tsk(ENEMY);					//敵0配置
	act_tsk(ENEMY2);				//敵1配置
	embdio_lcd_puts(true,0,0,"game start");
	gameover_count=3;				//gameover可能回数
	//メインループ
	while(1){
		sw=embdio_sw_sns();		//SW受付
		s1 = (sw & 0x01)? 1:0;
		s2 = (sw & 0x02)? 1:0;
		s3 = (sw & 0x04)? 1:0;
		s4 = (sw & 0x08)? 1:0;

		jump();					//ジャンプ
		move();					//左右移動
		mario_anime();			//マリオ描画

		foot_line(150,250,170); //足場作成
		foot_line(350,400,102);
		foot_line(50,150,100);
		foot_line(100,300,50);

		enemy_touch(0);			//敵0と接触した時の処理
		enemy_touch(1);			//敵1と接触した時の処理
		display_object();		//object(foot,coin)描画
		display_message();		//message表示
		dly_tsk(4);				//割り込み処理の為のディレイ
	}
	ext_tsk();
}

//ジャンプ動作
void jump(void)
{
	if(!fall_down && s1){						//落下フラグなければ、s1でジャンプ可能
		jumping = 1;
		mario_y += 2;
		jump_counter++;
	}
	if(jumping){									//ジャンプ中、常に下へ
		if(mario_y)mario_y -= 1;
		else jumping = fall_down = 0;
	}
	//jump_counter(頂点)で落下flgON,または,マリオが落下し始めたら落下flgON
	if((jump_counter==90)||(mario_y_re > mario_y)){
		fall_down = 1;
		jump_counter = 0;
	}
}

//左右移動
void move(void)
{
	if(s3){
		mario_direction=1;						//マリオを右向きへ
		if(mario_x >= 460) mario_x += 0;		//画面端侵入禁止
		else if(s2) mario_x += 2;				//Bダッシュ
		else mario_x += 1;
	}
	else if(s4){
		mario_direction=0;						//マリオを左向きへ
		if(mario_x <= 2) mario_x += 0;			//画面端侵入禁止(x<0にならないよう調整)
		else if(s2) mario_x -= 2;				//Bダッシュ
		else mario_x -= 1;
	}
}

///マリオ描画
void mario_anime(void)
{
	//マリオが移動したら、直前の位置をboxでクリア
	if(mario_x != mario_x_re || mario_y != mario_y_re ){
		lcd_box(mario_x_re,mario_y_re,mario_x_re+18,mario_y_re+17,LCD_WHITE,true);
	}
	if(mario_direction==1)lcd_bitmap(mario_x,mario_y,mario,LCD_WHITE);	//右マリオ描画
	else lcd_bitmap(mario_x,mario_y,marioL,LCD_WHITE);					//左マリオ描画
	mario_x_re = mario_x;												//現在の状態記録
	mario_y_re = mario_y;												//現在の状態記録
}

//敵との接触
void enemy_touch(UINT id)
{
	if(touch(enemy_x[id],enemy_y[id]+15,enemy_x[id]+15,enemy_y[id])){
		act_tsk(RESET);
	}
}

//足場の作成
void foot_line(UINT left,UINT right,UINT high)
{
	lcd_line(left+10,high,right,high,LCD_BLACK);
	if(mario_y == high){
		if(left<mario_x && right>mario_x){ //一段に足場1つまで
			jumping = fall_down = 0;
		}else{
			jumping = 1;
		}
	}
}

//objectの描写
void display_object(void)
{
	volatile UINT i;				//coinカウント用変数

	//コインの表示
	for(i=0;i<8;i++){						//coin数分、繰り返し
		if(!touched[i]){					//取得フラグたってないコインは表示
			coin(coin_x[i],coin_y[i],i);
		}
	}
}

//コイン表示
void coin(UINT x,UINT y,UINT id)
{
	if(touch(x-2,y+3,x+2,y-3)){									//coinに接触したら
		touched[id]=1;									//接触フラグON
		led_operation();								//LED関数へ
		lcd_box(x-2,y-3,x+2,y+3,LCD_WHITE,true);		//描写のクリア
	}else{											//接触してなかったらcoin描画続ける
		lcd_box(x-2,y-3,x+2,y+3,LCD_YELLOW,true);
		lcd_box(x-2,y-3,x+2,y+3,LCD_BLACK,false);
		lcd_line(x,y-2,x,y+2,LCD_WHITE);
	}
}


//接触判定
BOOL touch(UINT x_left,UINT y_top,UINT x_right,UINT y_bottom)
{
	if( mario_x+15 > x_left			//mario右値より、引数x左値が小さい(＝接触)
		&& mario_x < x_right		//mario左値より、引数x右値が大きい(＝接触)
		&& mario_y+17 > y_bottom	//mario上値より、引数y下値が小さい(＝接触)
		&& mario_y < y_top			//mario下値より、引数y上値が大きい(＝接触)
		)return 1;	//すべて接触で1
	else return 0;	//そうでなければ0
}

//ledを点灯するだけ
void led_operation(void)
{
	embdio_led(led_count+3,true);		//coin取得でLED点灯
	led_count++;						//LED点灯カウントON
}

//messageを表示するよ
void display_message(void)
{
	if(led_count>=8){
		lcd_bitmap(180,100,game_complete,LCD_WHITE);
	}
	if(gameover_count==3){
		lcd_bitmap(0,255,mario,LCD_WHITE);
		lcd_bitmap(15,255,mario,LCD_WHITE);
	}else if(gameover_count==2){
		lcd_bitmap(0,255,mario,LCD_WHITE);
		lcd_box(15,255,30,271,LCD_WHITE,true);
	}
	else lcd_box(0,255,45,271,LCD_WHITE,true);
}

