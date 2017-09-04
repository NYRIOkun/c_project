#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *func1(void *x);
void *func2(void *x);

int main(void)
{
    int *ret;
    pthread_t t1;
    pthread_t t2;
    
    pthread_create(&t1,NULL,func1,(void *)1);
    pthread_create(&t2,NULL,func2,(void *)2);
    printf("main()\n");
    pthread_join(t1,(void **)&ret);
    printf("func1 ret=%d\n",*ret);
    pthread_join(t2,(void **)&ret);
    printf("func2 ret=%d\n",*ret);
    return 0;
}

void *func1(void *x)
{
    int i;
    int y;
    static int ret;
    
    y=(int)x;
    for(i=0;i<5;i++){
        printf("func1(%d):%d \n",y,i);
        usleep(300000);
    }
    ret=10;
    pthread_exit(&ret);
}

void *func2(void *x)
{
    int i;
    int y;
    static int ret;
    
    y=(int)x;
    for(i=0;i<5;i++){
        printf("func2(%d):%d \n",y,i);
        usleep(500000);
    }
    ret=20;
    pthread_exit(&ret);
}
