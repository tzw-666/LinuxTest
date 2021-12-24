#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUF_LEN 1024

int main(void) {
	int fd = 0;
	ssize_t readLen = 0, writeLen = 0;
	char *fileName = "fileTest.txt";
	char *content = "This is my school number:19407050131\n";
	char readBuf[BUF_LEN];
	ssize_t contentLen = strlen(content);
	if ((fd = open(fileName, O_CREAT|O_RDWR, 0777)) < 0) {
		perror("open_error");
		exit(0);
	}
	lseek(fd, 0, SEEK_END);
	writeLen = write(fd, content, contentLen);
	if (writeLen == contentLen) {
		printf("write successfully\n");
	} else if (writeLen < 0) {
		perror("write_error");
	} else {
		printf("write partitially\n");
	}	
	lseek(fd, 0, SEEK_SET);
	readLen = read(fd, readBuf, sizeof(readBuf));
	if (readLen < 0) {
		perror("read");
	} else {
		printf("Current content:\n%s\n", readBuf);
	}
	close(fd);
	return 0;
}
