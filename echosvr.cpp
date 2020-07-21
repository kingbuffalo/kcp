#include <iostream>
#include <unistd.h>

#include <cstdlib>
#include <cstring>

#include "ikcp.h"
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

#define PORT 9991
#define MAXLEN 4096

int sockfd;

static inline void itimeofday(long *sec, long *usec)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
}

static inline IINT64 iclock64(void)
{
	long s, u;
	IINT64 value;
	itimeofday(&s, &u);
	value = ((IINT64)s) * 1000 + (u / 1000);
	return value;
}

static inline IUINT32 iclock()
{
	return (IUINT32)(iclock64() & 0xfffffffful);
}

void echo(int sockfd,struct sockaddr* client, socklen_t clilen,ikcpcb* kcp1){
	int n;
	char msg[MAXLEN];
	socklen_t len;
	for(;;){
		len = clilen;
		ikcp_update(kcp1,iclock());
		while(1){
			n = recvfrom(sockfd,msg,MAXLEN,0,client,&len);
			if ( n < 0 ) break;
			ikcp_input(kcp1,msg,n);
		}
		while(1){
			n = ikcp_recv(kcp1,msg,MAXLEN);
			if ( n < 0  ) break;
			cout << "len="<< n << ",msg:" << msg << endl;
			ikcp_send(kcp1,msg, n);
		}
		//sendto(sockfd,msg,n,0,client,len);
		usleep(10*1000);
	}
}

int udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
	sendto(sockfd,buf,len,0,(struct sockaddr *)user,sizeof(struct sockaddr));
	return 0;
}

int main(int argc, const char *argv[])
{
	if ( argc <=1  ){
		cout << "need args:  ./echosvr 0.0.0.0" << endl;
		exit(0);
	}
	const char *ip = argv[1];
	cout << "echo svr start" <<endl;
	struct sockaddr_in svraddr,cliAddr;

	ikcpcb *kcp1 = ikcp_create(0x11223344, (void*)&cliAddr);
	kcp1->output = udp_output;
	ikcp_wndsize(kcp1, 128, 128);
	ikcp_nodelay(kcp1, 0, 10, 0, 0);

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	int flags = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);
	memset(&svraddr,0,sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	//svraddr.sin_addr.s_addr = htonl("0.0.0.0");
	svraddr.sin_addr.s_addr =inet_addr(ip);
	svraddr.sin_port = htons(PORT);

	bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr));
	echo(sockfd,(struct sockaddr*)&cliAddr,sizeof(cliAddr),kcp1);

	return 0;
}
