#include <iostream>

#include <cstdlib>
#include <cstring>
#include "ikcp.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

using namespace std;

#define PORT 9991
#define MAXLEN 4096

void echo(int sockfd,struct sockaddr* client, socklen_t clilen){
	int n;
	char msg[MAXLEN];
	socklen_t len;
	for(;;){
		len = clilen;
		n = recvfrom(sockfd,msg,MAXLEN,0,client,&len);
		sendto(sockfd,msg,n,0,client,len);
		cout << msg << endl;
	}
}

int main(int argc, const char *argv[])
{
	cout << "echo svr start" <<endl;
	int sockfd;
	struct sockaddr_in svraddr,cliAddr;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	memset(&svraddr,0,sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	//svraddr.sin_addr.s_addr = htonl("0.0.0.0");
	svraddr.sin_addr.s_addr =inet_addr("127.0.0.1");
	svraddr.sin_port = htons(PORT);

	bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr));
	echo(sockfd,(struct sockaddr*)&cliAddr,sizeof(cliAddr));

	return 0;
}
