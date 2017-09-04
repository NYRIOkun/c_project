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

//�ץ�ȥ��������
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


//�����ѿ����
UW sw;							//sw
ER er;							//er
UINT s1,s2,s3,s4;				//sw�������
UINT mario_x=0,mario_y=0;		//���ߤΥޥꥪ�ΰ���
UINT mario_x_re=0,mario_y_re=0;	//ľ���Υޥꥪ�ΰ���
UINT jump_counter=0;			//��������Υ�����
BOOL mario_direction=1;         //�ޥꥪ�θ����ե饰
BOOL fall_down=0;				//���ե饰
BOOL jumping=0;					//jump��ե饰
BOOL led_on=0;					//LEDɽ���ե饰
UINT led_count;					//coin�������ե饰
UINT gameover_count;			//gameover�ե饰
BOOL title_message=0;			//title�Υ�å������ե饰

//�����������
BOOL touched[]={0,0,0,0,0,0,0,0}; 						//coinɽ���ե饰����
UINT coin_x[]={ 50,100,150,200,250,300,350,430};		//coin x��(����470)
UINT coin_y[]={250,150, 40,200,250, 20, 80,260};		//coin y��(����270)

UINT enemy_x[] = {400,  0};								//Ũ�ΰ�������
UINT enemy_y[] = {200,180};
UINT enemy_x_re[] = {400,  0};
UINT enemy_y_re[] = {200,180};

//�����ȥ���̥�����
void title_task(VP_INT exinf)
{
	UW x=0,y=0;//tsc�ե饰
	if(!title_message){
		lcd_bitmap(180,100,title,LCD_WHITE);
		embdio_lcd_puts(true,0,0,"game title");
	}else{
		lcd_bitmap(180,130,gameover_message,LCD_WHITE);
		embdio_lcd_puts(true,0,0,"game over!");
	}

	while(1){
		er=tsc_wait(&x,&y,TMO_FEVR);
		if(er==E_OK && x && y){						//�������饿�å����줿�鳫��
			syslog(LOG_INFO,"x=%d y=%d",x,y);
			lcd_clr(LCD_WHITE);
			wup_tsk(MAIN_TASK);
			ext_tsk();
		}
	}
	ext_tsk();
}

//7seg timer������
void timer_task(VP_INT exinf)
{
	UINT time=24;

	while(1){
		embdio_7seg(2,true,time/10,false);
		embdio_7seg(1,true,time%10,false);
		dly_tsk(1000);
		time--;
		if(time==0){				//TIME0�ǥꥻ�åȥ�������ư
			time=24;
			act_tsk(RESET);
		}
	}
	ext_tsk();
}

//reset������
void reset_task(VP_INT exinf)
{
	volatile UINT i;
	mario_x=0,mario_y=0;		//���ߤΥޥꥪ�ΰ���
	mario_x_re=0,mario_y_re=0;	//ľ���Υޥꥪ�ΰ���
	jump_counter=0;				//��������Υ�����
	mario_direction=1;       	//�ޥꥪ�θ����ե饰
	fall_down=0;				//���ե饰
	jumping=0;					//jump��ե饰
	led_on=0;					//LEDɽ���ե饰
	led_count=0;				//coin�������ե饰
	enemy_x[0]=400;				//Ũ�ν������
	enemy_y[0]=0;
	enemy_x[1]=200;
	enemy_y[1]=180;

	for(i=0;i<8;i++){
		touched[i]=0;			//coin�����ե饰�ꥻ�å�
		embdio_led(3+i,false);
	}
	lcd_clr(LCD_WHITE);			//�����̤Υ��ꥢ
	gameover_count--;
	if(gameover_count==0){		//game_over�ե饰�����䤹
		title_message=1;
		act_tsk(GAMEOVER);
	}
	ext_tsk();
}

//enemy������
void enemy_task(VP_INT exinf)
{
	BOOL flag=0;
	while(1){
		if(enemy_x[exinf]==2)flag=1;	//��ü�Ǻ�ž��
		if(enemy_x[exinf]==460)flag=0;	//��ü�Ǳ�ž��
		enemy_x[exinf] +=(!flag? -1:1);	//flag�˱����ư�ư
		dly_tsk(2*(exinf+1));

		//enemy��ɽ������
		if(enemy_x[exinf] != enemy_x_re[exinf]){
			lcd_box(enemy_x_re[exinf],enemy_y_re[exinf],
					enemy_x_re[exinf]+18,enemy_y_re[exinf]+17,LCD_WHITE,true);
		}
		lcd_bitmap(enemy_x[exinf],enemy_y[exinf],poison,LCD_WHITE);
		enemy_x_re[exinf] = enemy_x[exinf];									//���ߤξ��ֵ�Ͽ
		enemy_y_re[exinf] = enemy_y[exinf];
	}
	ext_tsk();
}

//game over������
void gameover_task(VP_INT exinf)
{
  ter_tsk(MAIN_TASK);
  ter_tsk(ENEMY);
  ter_tsk(ENEMY2);
  ter_tsk(TIME);
  act_tsk(MAIN_TASK);
  ext_tsk();
}

//main����
void main_task(VP_INT exinf)
{
	syslog_msk_log(LOG_UPTO(LOG_INFO),LOG_UPTO(LOG_EMERG));
	syslog(LOG_INFO,"start program");
	syslog(LOG_INFO,"�ǥХå����������ϡ����Υ�å������򸡾ڲս���������Ƥ�������");

	act_tsk(RESET);					//������»�
	embdio_7seg(2,false,0,false);	//7seg����
	embdio_7seg(1,false,0,false);
	act_tsk(TITLE);					//�����ȥ���̤ذܹ�
	slp_tsk();						//��ư�����ޤǥ��꡼��
	act_tsk(TIME);					//�����ޡ���ư
	act_tsk(ENEMY);					//Ũ0����
	act_tsk(ENEMY2);				//Ũ1����
	embdio_lcd_puts(true,0,0,"game start");
	gameover_count=3;				//gameover��ǽ���
	//�ᥤ��롼��
	while(1){
		sw=embdio_sw_sns();		//SW����
		s1 = (sw & 0x01)? 1:0;
		s2 = (sw & 0x02)? 1:0;
		s3 = (sw & 0x04)? 1:0;
		s4 = (sw & 0x08)? 1:0;

		jump();					//������
		move();					//������ư
		mario_anime();			//�ޥꥪ����

		foot_line(150,250,170); //­�����
		foot_line(350,400,102);
		foot_line(50,150,100);
		foot_line(100,300,50);

		enemy_touch(0);			//Ũ0���ܿ��������ν���
		enemy_touch(1);			//Ũ1���ܿ��������ν���
		display_object();		//object(foot,coin)����
		display_message();		//messageɽ��
		dly_tsk(4);				//�����߽����ΰ٤Υǥ��쥤
	}
	ext_tsk();
}

//������ư��
void jump(void)
{
	if(!fall_down && s1){						//��ե饰�ʤ���С�s1�ǥ����ײ�ǽ
		jumping = 1;
		mario_y += 2;
		jump_counter++;
	}
	if(jumping){									//�������桢��˲���
		if(mario_y)mario_y -= 1;
		else jumping = fall_down = 0;
	}
	//jump_counter(ĺ��)���flgON,�ޤ���,�ޥꥪ������Ϥ᤿���flgON
	if((jump_counter==90)||(mario_y_re > mario_y)){
		fall_down = 1;
		jump_counter = 0;
	}
}

//������ư
void move(void)
{
	if(s3){
		mario_direction=1;						//�ޥꥪ�򱦸�����
		if(mario_x >= 460) mario_x += 0;		//����ü�����ػ�
		else if(s2) mario_x += 2;				//B���å���
		else mario_x += 1;
	}
	else if(s4){
		mario_direction=0;						//�ޥꥪ�򺸸�����
		if(mario_x <= 2) mario_x += 0;			//����ü�����ػ�(x<0�ˤʤ�ʤ��褦Ĵ��)
		else if(s2) mario_x -= 2;				//B���å���
		else mario_x -= 1;
	}
}

///�ޥꥪ����
void mario_anime(void)
{
	//�ޥꥪ����ư�����顢ľ���ΰ��֤�box�ǥ��ꥢ
	if(mario_x != mario_x_re || mario_y != mario_y_re ){
		lcd_box(mario_x_re,mario_y_re,mario_x_re+18,mario_y_re+17,LCD_WHITE,true);
	}
	if(mario_direction==1)lcd_bitmap(mario_x,mario_y,mario,LCD_WHITE);	//���ޥꥪ����
	else lcd_bitmap(mario_x,mario_y,marioL,LCD_WHITE);					//���ޥꥪ����
	mario_x_re = mario_x;												//���ߤξ��ֵ�Ͽ
	mario_y_re = mario_y;												//���ߤξ��ֵ�Ͽ
}

//Ũ�Ȥ��ܿ�
void enemy_touch(UINT id)
{
	if(touch(enemy_x[id],enemy_y[id]+15,enemy_x[id]+15,enemy_y[id])){
		act_tsk(RESET);
	}
}

//­��κ���
void foot_line(UINT left,UINT right,UINT high)
{
	lcd_line(left+10,high,right,high,LCD_BLACK);
	if(mario_y == high){
		if(left<mario_x && right>mario_x){ //���ʤ�­��1�Ĥޤ�
			jumping = fall_down = 0;
		}else{
			jumping = 1;
		}
	}
}

//object������
void display_object(void)
{
	volatile UINT i;				//coin����������ѿ�

	//�������ɽ��
	for(i=0;i<8;i++){						//coin��ʬ�������֤�
		if(!touched[i]){					//�����ե饰���äƤʤ��������ɽ��
			coin(coin_x[i],coin_y[i],i);
		}
	}
}

//������ɽ��
void coin(UINT x,UINT y,UINT id)
{
	if(touch(x-2,y+3,x+2,y-3)){									//coin���ܿ�������
		touched[id]=1;									//�ܿ��ե饰ON
		led_operation();								//LED�ؿ���
		lcd_box(x-2,y-3,x+2,y+3,LCD_WHITE,true);		//���̤Υ��ꥢ
	}else{											//�ܿ����Ƥʤ��ä���coin����³����
		lcd_box(x-2,y-3,x+2,y+3,LCD_YELLOW,true);
		lcd_box(x-2,y-3,x+2,y+3,LCD_BLACK,false);
		lcd_line(x,y-2,x,y+2,LCD_WHITE);
	}
}


//�ܿ�Ƚ��
BOOL touch(UINT x_left,UINT y_top,UINT x_right,UINT y_bottom)
{
	if( mario_x+15 > x_left			//mario���ͤ�ꡢ����x���ͤ�������(���ܿ�)
		&& mario_x < x_right		//mario���ͤ�ꡢ����x���ͤ��礭��(���ܿ�)
		&& mario_y+17 > y_bottom	//mario���ͤ�ꡢ����y���ͤ�������(���ܿ�)
		&& mario_y < y_top			//mario���ͤ�ꡢ����y���ͤ��礭��(���ܿ�)
		)return 1;	//���٤��ܿ���1
	else return 0;	//�����Ǥʤ����0
}

//led�������������
void led_operation(void)
{
	embdio_led(led_count+3,true);		//coin������LED����
	led_count++;						//LED�����������ON
}

//message��ɽ�������
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

