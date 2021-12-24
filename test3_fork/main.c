#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "sys/wait.h"
#include "fcntl.h"
#include "stdlib.h"

#define BUF_LEN 1024

int main(void) {
	int fd = 0;
    pid_t pid;
	ssize_t readLen = 0, writeLen = 0;
	char *fileName = "19407050131.txt";
	char *content = "My school number is:19407050131\n";
	ssize_t contentLen = strlen(content);
    if ((fd = open(fileName, O_CREAT|O_RDWR, 0777)) < 0) {
		perror("create_error");
		exit(0);
	}
    close(fd);
    pid = fork();
    if (pid > 0) {
        if ((fd = open(fileName, O_WRONLY)) < 0) {
            perror("open_error\n");
            exit(-1);
        }
        printf("Process:%d start to write file\n", getpid());	
        lseek(fd, 0, SEEK_END);
        writeLen = write(fd, content, contentLen);
        if (writeLen == contentLen) {
            printf("write successfully\n");
            printf("write %dBytes\n", writeLen);
        } else if (writeLen < 0) {
            perror("write_error\n");
        } else {
            printf("write partitially\n");
        }
        close(fd);
        wait(NULL);
    } else if (pid == 0) {
        if (fork() == 0) {
            char readBuf[BUF_LEN];
            sleep(2);
            if ((fd = open(fileName, O_RDONLY)) < 0) {
                perror("open_error\n");
                exit(-1);
            }
            printf("Process:%d start to read file\n", getpid());	
            lseek(fd, 0, SEEK_SET);
            readLen = read(fd, readBuf, sizeof(readBuf));
            if (readLen < 0) {
                perror("read_error\n");
            } else {
                readBuf[readLen] = '\0';
                printf("Current content:\n%s\n", readBuf);
                printf("length:%d\n", readLen);
            }
            close(fd);
        } else {
            wait(NULL);
        }
    } else {
        perror("fork error");
    }
	return 0;
}


