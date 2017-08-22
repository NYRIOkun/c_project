#include <unistd.h> //for fork()
#include <linux/input.h> //structの中身はこの中にあるよ
#include <sys/stat.h>

int main(void)
{
    mkfifo("testfifo",0600);
    perror("FIFO");
    return 0;
}
