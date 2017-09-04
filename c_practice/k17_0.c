#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define SIZE 20
// DBファイル名
const char *fileName = "database.db";

//テーブル・ビュー作成用SQL
const char *cre_tbl[]={
"CREATE TABLE IF NOT EXISTS tb_ev(ev_id INTEGER PRIMARY KEY, ev_tm TEXT, ev_op INTEGER, rm_id INTEGER)",
"CREATE TABLE IF NOT EXISTS tb_rm(rm_id INTEGER PRIMARY KEY, rm_room TEXT, lp_id INTEGER, rm_tm TEXT)",
"CREATE TABLE IF NOT EXISTS tb_lp(lp_id INTEGER PRIMARY KEY, lp_nm TEXT, lp_w INTEGER, lp_th INTEGER, lp_st INTEGER)",
"CREATE VIEW IF NOT EXISTS v_rmlp AS SELECT tb_rm.rm_room,tb_lp.lp_nm,tb_lp.lp_w FROM tb_rm,tb_lp WHERE tb_rm.lp_id = tb_lp.lp_id",
NULL};

//tb_rm用の構造体定義
typedef struct {
	int rm_id;
	char *rm_room;
	int lp_id;
	char rm_tm[SIZE]; // yyyy/mm/dd hh:mm:ss\0 20bytes
}INS_DATA_RM;

//tb_rm用のデータ
const INS_DATA_RM ins_data_rm[]={
	{1,"玄関",1,"2015/03/02 12:34:00"},
	{2,"トイレ",1,"2016/01/3 18:25:00"},
	{3,"居間",2,"2013/4/01 15:44:00"},
	{4,"寝室",2,"2015/6/07 17:13:00"},
	{0,NULL,0,""}
};

//tb_lp用の構造体定義（未使用）
typedef struct {
	int lp_id;
	char *lp_nm;
	int lp_w;
	int lp_th;
	int lp_st;
}INS_DATA_LP;

//tb_ev用の構造体定義（未使用）
typedef struct {
	int ev_id;
	char ev_tm[SIZE]; // yyyy/mm/dd hh:mm:ss\0 20bytes
	int ev_op;
	int rm_id;
}INS_DATA_EV;

int main(void) {
	char *errMsg = NULL;
	int err = 0;
	int i;
	char buff[SIZE];

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
	
	// テーブルの作成
	printf("テーブルの作成\n");
	i=0;
	while(cre_tbl[i]!=NULL){
		err = sqlite3_exec(pDB, cre_tbl[i], NULL, NULL, &errMsg);
		//エラー処理
		if(err != SQLITE_OK){
			printf("%s\n", errMsg);
			sqlite3_free(errMsg);
			errMsg = NULL;
		}
		i++;
	}
	
	// データの追加
	printf("データの追加\n");
	// tb_rm用ステートメントの用意
	err = sqlite3_prepare_v2(pDB,"INSERT INTO tb_rm VALUES(?,?,?,?)",-1, &pStmt, NULL);
	
	if(err != SQLITE_OK){
		printf("sqlite3_prepare_v2  ERR! %d \n",err);
	}else{
		printf("sqlite3_prepare_v2 OK!\n");
		i = 0;
		while(ins_data_rm[i].rm_id>0){
		
		// ステートメントの?の部分に型変換して値を代入
		sqlite3_bind_int(pStmt, 1, ins_data_rm[i].rm_id);
		sqlite3_bind_text(pStmt, 2, ins_data_rm[i].rm_room,
			strlen(ins_data_rm[i].rm_room), SQLITE_TRANSIENT);
		sqlite3_bind_int(pStmt, 3, ins_data_rm[i].lp_id);
		sqlite3_bind_text(pStmt, 4, ins_data_rm[i].rm_tm, 
			strlen(ins_data_rm[i].rm_tm), SQLITE_TRANSIENT);
		//SQLITE_BUSYの間は繰り返し
		while(SQLITE_BUSY ==(err= sqlite3_step(pStmt))){}
		printf("sqlite3_step　err=%d\n",err);
		//エラー処理（主キー重複エラー）
		if(err == SQLITE_CONSTRAINT){
			printf("%s\n",sqlite3_errmsg(pDB));
		}
		//ステートメントの初期化
		sqlite3_reset(pStmt);
		i++;
		}
	}
	
	// ステートメントの解放
	sqlite3_finalize(pStmt);
	
	// データの抽出
	printf("データの抽出\n");
	// ステートメントの用意
	err = sqlite3_prepare_v2(pDB,"SELECT * FROM tb_rm", -1,&pStmt, NULL);
	//エラーチェック
	if(err != SQLITE_OK){
		printf("sqlite3_prepare_v2  ERR! %d \n",err);
	}else{
		// データの抽出
		while(SQLITE_ROW == (err = sqlite3_step(pStmt)) ){
			printf("Room: %d-%s Lamp: %d Time: %s\n", 
			sqlite3_column_int(pStmt, 0),sqlite3_column_text(pStmt, 1),
			sqlite3_column_int(pStmt, 2),sqlite3_column_text(pStmt, 3));
		}
		//エラー処理
		if(err != SQLITE_DONE){
			printf("sqlite3_step ERR! %d \n",err);
		}
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
