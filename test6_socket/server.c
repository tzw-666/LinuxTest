#include "sys/ipc.h"
#include "sys/shm.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "sys/fcntl.h"
#include "stdlib.h"
#include "string.h"
#include "netinet/in.h"
#include "unistd.h"
#include "time.h"
#include "arpa/inet.h"
#include "stdio.h"

void sockAddrInit (struct sockaddr_in *sockAddr, sa_family_t family, const char *addr, in_port_t port);

int main (void) {
    
    int sockFd, connFd;
    void *temp;
    char *shmBuf;
    char *connNum, *status;
    struct sockaddr_in sockAddr, connAddr;
    int addrLen = sizeof(struct sockaddr_in);

    int shmId;
    pid_t pid;

    shmId = shmget(12345, 10240, 0666 | IPC_CREAT);
    
    if (shmId == -1) {
        perror("shmget error\n");
    }
    temp = shmat(shmId, 0, 0);

    memset(temp, 0, 10240);
    status = (char*)temp;
    connNum = (char*)temp + 1;
    shmBuf = (char*)temp + 2;

    *status = 1;
    *connNum = 0;

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket create error\n");
    }
 
    sockAddrInit(&sockAddr, AF_INET, INADDR_ANY, 9999);
    
    if (bind(sockFd, (struct sockaddr *)&sockAddr, addrLen) == -1) {
        perror("bind error\n");
        exit(-1);
    }
    
    if (listen(sockFd, 5) == -1) {
        perror("listen error\n");
        exit(-1);
    }
    printf("start listenning\n");

    //服务器终端
    pid = getpid();
    if (fork() == 0) {
        char buf[128];
        int bufLen;
        printf("control terminal start\n");
        
        while (*status) {
            printf("terminal:");
            gets(buf);
            if (!strcmp(buf, "close")) {
                printf("sure by type yes:");
                gets(buf);
                if (!strcmp(buf, "yes")) {
                    *status = 0;
                    printf("close the server now\n");
                    usleep(100 * 1000);
                }
            } else if (!strcmp(buf, "num -c")) {
                printf("connect num:%d\n", *connNum);
            } else if (strlen(buf) > 0) {
                printf("can not find the command\n");
            }
        }
        shmdt(temp);
        shmctl(shmId, IPC_RMID, NULL);
        printf("share memory has been removed\n");
        bufLen = sprintf(buf, "kill %d", pid);
        buf[bufLen] = 0;
        system(buf);
        exit(0);
    }
    
    //服务器收发
    while (*status) {
        connFd = accept(sockFd, (struct sockaddr *)&connAddr, &addrLen);
        if (connFd == -1) {
            perror("accept error\n");
            shmctl(shmId, IPC_RMID, NULL);
            exit(-1);
        }
        pid = fork();
        if (pid == 0) {
            int i = 0, readLen;
            char buf[128];
            ++(*connNum);
            close(sockFd);
            pid = fork();
            if (pid > 0) {
                while (*status) {
                    readLen = read(connFd, buf, 128 - 1);
                    if (readLen > 0) {
                        buf[readLen] = 0;
                        for (; shmBuf[i * 128] != 0; ++i);
                        strcpy(shmBuf + i * 128, buf);
                    }
                }
            } else if (pid == 0) {
                while (*status) {
                    while (shmBuf[i * 128] != 0) {
                        write(connFd, shmBuf + i * 128, strlen(shmBuf + i * 128));
                        ++i;
                    }
                }
            }
            shmdt(temp);
            exit(0);
        } else if (pid > 0) {
            close(connFd);
        } else {
            perror("fork error\n");
            exit(-1);
        }
    }
    return 0;
}

void sockAddrInit (struct sockaddr_in *sockAddr, sa_family_t family, const char *addr, in_port_t port) {
    memset(&(*sockAddr->sin_zero), 0, sizeof(sockAddr->sin_zero));
    sockAddr->sin_family = family;
    if (addr == (char *)INADDR_ANY) {
        sockAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        sockAddr->sin_addr.s_addr = inet_addr(addr);
    }
    sockAddr->sin_port = htons(port);
}

