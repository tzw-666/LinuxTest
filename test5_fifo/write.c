#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

const char fifo[] = "myFifo";

int main (void) {
    int pid;
    int fd;
    char buf[1024] = {0};
    int writeLen;
    unlink(fifo);
    if (mkfifo(fifo, 0666)) {
        perror("create fifo error\n");
        exit(1);
    }
    fd = open(fifo, O_WRONLY);
    if (fd > 0) {
        printf("open fifo successfully\n");
        printf("please input message end with Enter\n");
        while (strcmp("byebye", buf)) {
            gets(buf);
            write(fd, buf, strlen(buf));
        }
    } else {
        perror("open fifo error\n");
    }
    return 0;
}