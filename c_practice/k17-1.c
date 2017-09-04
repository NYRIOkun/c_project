#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define SIZE 20
// DBファイル名
const char *fileName = "shoumei2.db";

//tb_rm用の構造体定義
typedef struct {
	int rm_id;
	char *rm_room;
	int lp_id;
	char rm_tm[SIZE]; // yyyy/mm/dd hh:mm:ss\0 20bytes
}INS_DATA_RM;

//tb_lp用の構造体定義（未使用）
typedef struct {
	int lp_id;
	char *lp_nm;
	int lp_w;
	int lp_th;
	int lp_st;
}INS_DATA_LP;

//★tb_lp用のデータ
const INS_DATA_LP ins_data_lp[]={
	{1,"IL-001",60,1000,1},
	{2,"FL-001",72,16000,2},
	{0,NULL,0,0,0}
};

//tb_ev用の構造体定義（未使用）
typedef struct {
	int ev_id;
	char ev_tm[20]; // yyyy/mm/dd hh:mm:ss\0 20bytes
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

	// ランプデータの追加
	printf("ランプデータの追加\n");
	// tb_lp用ステートメントの用意
	err = sqlite3_prepare_v2(pDB,"INSERT INTO tb_lp VALUES(?,?,?,?,?)",-1, &pStmt, NULL);

	if(err != SQLITE_OK){
		printf("sqlite3_prepare_v2  ERR! %d \n",err);
	}else{
		printf("sqlite3_prepare_v2 OK!\n");
		i = 0;
		while(ins_data_lp[i].lp_id>0){
		//★ ステートメントの?の部分に型変換して値を代入
		sqlite3_bind_int(pStmt, 1, ins_data_lp[i].lp_id);
		sqlite3_bind_text(pStmt, 2, ins_data_lp[i].lp_nm,strlen(ins_data_lp[i].lp_nm), SQLITE_TRANSIENT);
		sqlite3_bind_int(pStmt, 3, ins_data_lp[i].lp_w);
		sqlite3_bind_int(pStmt, 4, ins_data_lp[i].lp_th);
		sqlite3_bind_int(pStmt, 5, ins_data_lp[i].lp_st);

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
	err = sqlite3_prepare_v2(pDB,"SELECT * FROM tb_lp", -1,&pStmt, NULL);
	//エラーチェック
	if(err != SQLITE_OK){
		printf("sqlite3_prepare_v2  ERR! %d \n",err);
	}else{
		// データの抽出
		while(SQLITE_ROW == (err = sqlite3_step(pStmt)) ){
			//★printfでの出力
			printf("電球:%d-%s 消費電力:%dW 寿命: %d時間 在庫:%d個\n",
			sqlite3_column_int(pStmt, 0),sqlite3_column_text(pStmt, 1),
			sqlite3_column_int(pStmt, 2),sqlite3_column_int(pStmt, 3),sqlite3_column_int(pStmt, 4));
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
