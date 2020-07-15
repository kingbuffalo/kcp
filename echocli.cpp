
#include <iostream>
#include <cstdlib>
#include <cstring>

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
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

int sockfd;
ikcpcb *kcp1 ;
struct sockaddr_in svraddr;


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


int udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
	sendto(sockfd,buf,len,0,(struct sockaddr *)user,len);
	return 0;
}

void* echo(void* arg){
	int n;
	char msg[MAXLEN];
	socklen_t len;
	len = sizeof(svraddr);
	for(;;){
		cout << "cli echo loop" <<endl;
		ikcp_update(kcp1,iclock());
		while(1){
			cout << "cli echo loop while1" << endl;
			n = recvfrom(sockfd,msg,MAXLEN,0,(struct sockaddr*)&svraddr,&len);
			if ( n < 0 ) break;
			ikcp_input(kcp1,msg,n);
		}
		while(1){
			cout << "cli echo loop while2" <<endl;
			n = ikcp_recv(kcp1,msg,MAXLEN);
			if ( n < 0  ) break;
			cout << msg << endl;
		}
		//sendto(sockfd,msg,n,0,client,len);
		usleep(10*1000);
	}
}

int main(int argc, const char *argv[])
{
	cout << "echo cli start" <<endl;
	srand((unsigned)time(NULL));

	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	bzero(&svraddr, sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(PORT);
	svraddr.sin_addr.s_addr =inet_addr("127.0.0.1");
	if ( connect(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr)) == -1){
		cout << "connect error" <<endl;
		exit(1);
	}

	kcp1 = ikcp_create(0x11223344, (void*)&svraddr);
	kcp1->output = udp_output;
	ikcp_wndsize(kcp1, 128, 128);
	ikcp_nodelay(kcp1, 0, 10, 0, 0);


	char writeMsg[MSGLEN] = {0};

	pthread_t  th1;
	if ( pthread_create(&th1,NULL,echo,NULL) ){
		cout << "create thread error" <<endl;
		return -1;
	}
	//char readMsg[MSGLEN+1] = {0};
	for(;;){
		sleep(1);
		for( int i=0;i<MSGLEN;i++ ){
			int ic = rand()%26;
			char c = 'a' + ic;
			writeMsg[i] = c;
		}
		//cout << "send   " << writeMsg <<endl;
		ikcp_send(kcp1,writeMsg, MSGLEN);
		/*
		   send(sockfd,writeMsg,MSGLEN,0);
		   int n = read(sockfd,readMsg,MSGLEN+1);
		   if ( n == -1 ){
		   cout << "read error" << endl;
		   exit(1);
		   }else{
		   cout << readMsg << endl;
		   }
		   */
	}
	pthread_join(th1,NULL);

	return 0;
}
