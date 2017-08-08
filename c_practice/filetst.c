#include<stdio.h>
#define IN_FILE_PATH "data.txt"
#define OUT_FILE_PATH "sum.txt"

int main(void)
{
    FILE *fp_r = NULL;
    FILE *fp_w = NULL;
    int read_data = 0;
    short sum = 0;
    
    printf("ファイルを開きます\n");
    if((fp_r = fopen(IN_FILE_PATH,"r")) == NULL){
        printf("ファイルオープン失敗\n");
        return 1;
    }
    
    while(fscanf(fp_r,"%d",&read_data) != EOF)
    {
        sum += read_data;
    }
        
    printf("読み込み完了\n");
    printf("ファイルを閉じます\n");
    
    fclose(fp_r);
    
    printf("ファイルを開きます\n");
    if((fp_w = fopen(OUT_FILE_PATH,"w")) == NULL){
        printf("ファイルオープン失敗\n");
        return 1;
    }
    
    fprintf(fp_w,"%d\n",sum);
    printf("ファイルへの書き込み完了\n");
    fclose(fp_w);
    
    return 0;
}