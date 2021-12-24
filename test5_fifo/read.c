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
    int readLen = 0;
    fd = open(fifo, O_RDONLY);
    if (fd > 0) {
        printf("fifo read start\n");
        while (strcmp("byebye", buf)) {
            readLen = read(fd, buf, 1024);
            buf[readLen] = '\0';
            printf("%s\n", buf);
        }
        close(fd);
    } else {
        perror("fifo create error\n");
        exit(1);
    }
    return 0;
}