
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "acl.h"

int main(void)
{
	acl_t *acl;
	struct sockaddr_in si;

	si.sin_family = AF_INET;
	si.sin_port = htons(20);
	si.sin_addr.s_addr = inet_addr("10.20.12.66");
	acl = acl_load_from_file("libwsocks.acl");
	if(!acl){
		printf("load error\n");
		return -1;
	}
	acl_do(acl, &si, NULL);
	acl_load_to_file(acl, "libwsocks.acl2");
	acl_load_to_FILE(acl, stdout);
	acl_free(acl);

	return 0;
}
