#include <iostream>

#include <cstdlib>
#include <cstring>
#include "ikcp.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <set>

using namespace std;

#define PORT 9981
#define MAXLEN 256


set<string> ipSet;

void echo(int sockfd,struct sockaddr_in* client, socklen_t clilen){
	int n;
	char msg[MAXLEN];
	socklen_t len;
	int cnt=0;
	for(;;){
		len = clilen;
		n = recvfrom(sockfd,msg,MAXLEN,0,(struct sockaddr*)client,&len);
		sendto(sockfd,msg,n,0,(struct sockaddr*)client,len);
		const char *ip = inet_ntoa(client->sin_addr);
		string str(ip);
		ipSet.insert(ip);
		if ( ++ cnt > 3600 ){
			cout << "ipCnt=" << ipSet.size() << endl;
			cnt = 0;
		}
		//cout << inet_ntoa(client->sin_addr) << endl;
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
	svraddr.sin_addr.s_addr =inet_addr("0.0.0.0");
	svraddr.sin_port = htons(PORT);

	bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr));
	echo(sockfd,&cliAddr,sizeof(cliAddr));

	return 0;
}
