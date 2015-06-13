
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define die(reason) \
	do{ \
		perror(reason); \
		exit(-1); \
	}while(0);

int main(void)
{
	int sock;
	struct sockaddr_in addr;
	struct sockaddr_in retaddr;
	socklen_t slen = sizeof(addr);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		die("socket");
	}
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("10.10.99.235");
	addr.sin_port = htons(80);
	if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		die("connect");
	}
	printf("connected\n");
	if(getpeername(sock, (struct sockaddr*)&retaddr, &slen) < 0){
		die("getpeername");
	}
	if(memcmp(&retaddr, &addr, sizeof(addr)) != 0) die("the addresses aren't the same");
	close(sock);

	return 0;
}
