#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main (void) {
    int pipe_fd[2];
    int pid;
    char writeBuf[1024];
    char readBuf[4096];
    int readLen;
    int writeLen;
    if (pipe(pipe_fd) < 0) {
        perror("create pipe error\n");
        exit(1);
    } else {
        printf("pipe has been created\n");
    }
    pid = fork();
    for (int i = 0; i < 100; ++i) {
        writeLen = sprintf(writeBuf, "%d(%3d):hello\n", getpid(), i + 1);
        write(pipe_fd[1], writeBuf, writeLen);
    }
    if (pid > 0) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        waitpid(pid, NULL, 0);
    } else if (pid == 0) {
        sleep(3);
        readLen = read(pipe_fd[0], readBuf, 4096);
        if (readLen > 0) {
            readBuf[readLen] = '\0';
            printf("process%dreceive content:\n%s\n", getpid(), readBuf);
        } else {
            printf("read error\n");
            exit(1);
        }
        close(pipe_fd[0]);
        close(pipe_fd[1]);
    } else {
        printf("fork error\n");
        exit(1);
    }
    return 0;
}



