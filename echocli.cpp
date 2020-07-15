
#include <iostream>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "ikcp.h"

using namespace std;

#define PORT 9991
//#define IP "59.110.22.107"
#define IP "127.0.0.1"
#define MAXLEN 4096
#define MSGLEN 256

int main(int argc, const char *argv[])
{
	cout << "echo cli start" <<endl;
	srand((unsigned)time(NULL));

	int sockfd;
	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in svraddr;
	bzero(&svraddr, sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(PORT);
	svraddr.sin_addr.s_addr =inet_addr("127.0.0.1");
	if ( connect(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr)) == -1){
		cout << "connect error" <<endl;
		exit(1);
	}
	char writeMsg[MSGLEN] = {0};
	char readMsg[MSGLEN+1] = {0};
	for(;;){
		sleep(1);
		for( int i=0;i<MSGLEN;i++ ){
			int ic = rand()%26;
			char c = 'a' + ic;
			writeMsg[i] = c;
		}
		send(sockfd,writeMsg,MSGLEN,0);
		int n = read(sockfd,readMsg,MSGLEN+1);
		if ( n == -1 ){
			cout << "read error" << endl;
			exit(1);
		}else{
			cout << readMsg << endl;
		}
	}

	return 0;
}
