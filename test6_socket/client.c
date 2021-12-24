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
#include "stddef.h"

int sockAddrInit(struct sockaddr_in *sockAddr, sa_family_t family, const char *addr);

char *getLocalTime();

int main(int argc, char **argv)
{
    int sockFd;
    int pid;
    int readLen, writeLen;
    char buf[128], name[20];
    struct sockaddr_in sockAddr;

    sockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (argc != 3 || strcmp(argv[1], "-c")) {
        printf("error command\n");
        exit(-1);
    }
    else if (sockAddrInit(&sockAddr, AF_INET, argv[2]) == -1) {
        printf("error addr\n");
        exit(-1);
    }
    printf("please put int your name:");
    gets(name);
    readLen = strlen(name);
    if (readLen < 1 || readLen > 20) {
        printf("set name error\n");
        exit(-1);
    }
    printf("now connect to %s, please wait ...\n", argv[2]);
    if (connect(sockFd, (struct sockaddr *)&sockAddr, sizeof(struct sockaddr_in)) == -1) {
        perror("connnect error\n");
        exit(-1);
    }

    system("clear");
    printf("chatting window start\n");

    pid = fork();

    if (pid > 0) {
        while (1) {
            readLen = read(sockFd, buf, 128 - 1);
            if (readLen > 0) {
                buf[readLen] = 0;
                printf("%s\n", buf);
            }
        }
    }
    else if (pid == 0)
    {
        char writeBuf[128];
        while (1)
        {
            gets(buf);
            if (strlen(buf) > 0)
            {
                writeLen = sprintf(writeBuf, "%s[%s]:%s", name, getLocalTime(), buf);
                write(sockFd, writeBuf, writeLen);
            }
        }
    }
    else
    {
        perror("fork error\n");
        exit(-1);
    }
    close(sockFd);
    return 0;
}

int sockAddrInit(struct sockaddr_in *sockAddr, sa_family_t family, const char *addr) {
    static char netAddr[16];
    int i = 0, port = 0;
    memset(&(*sockAddr->sin_zero), 0, sizeof(sockAddr->sin_zero));
    sockAddr->sin_family = family;

    for (i = 0; (netAddr[i] = addr[i]) != ':' && i < 16; ++i);
    if (i == 16) {
        printf("addr error\n");
        return -1;
    }
    netAddr[i] = 0;
    printf("addr:%s\n", netAddr);
    sockAddr->sin_addr.s_addr = inet_addr(netAddr);

    for (port = 0, ++i; addr[i] != 0 && port < 65536; ++i) {
        port = port * 10 + addr[i] - '0';
    }
    if (port < 65536) {
        sockAddr->sin_port = htons(port);
        printf("port:%d\n", port);
    }
    else {
        printf("port error:%d\n", port);
        return -1;
    }
    return 0;
}

char *getLocalTime() {
    static char localTime[64];
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    strcpy(localTime, asctime(lt));
    localTime[strlen(localTime) - 1] = 0;
    return localTime;
}
