#include <stdio.h>

unsigned long c;

int main(void)
{
    c=0;
    while(c<1000000L){
        printf("oh...");
        c++;
    }
    return 0;
}