#include <stdio.h>
#include <sys/socket.h> //for socket(), connect(), send(), recv() functions
#include <arpa/inet.h>// different address structures are declared here
#include <stdlib.h> // atoi() which convert string to integer
#include <string.h>
#include <unistd.h> // close() function
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>

#define BUFSIZE 1000
#define PORT 3000
// char recvbuf[BUFSIZE];
// char sendbuf[BUFSIZE];
long long int i=0;
long long int i2=0;
FILE * fp;
// int sock;
int maxConns=5;
int nConns=0;
int nLeftToConnect;
int nsecs=2;
long long int numberOfRequests=0;

int max(int a, int b){
	return a>b?a:b;
}

void * func(void * arg	){
	int sock=*((int *)arg);
	int n;
	char recvbuf[BUFSIZE];

	while((n=recv(sock,recvbuf, sizeof(recvbuf),0))>0){
		i+=n;
		numberOfRequests++;
		int k=write(STDOUT_FILENO, recvbuf, sizeof(recvbuf));
		if(k<n){
			printf("WRITE ERROR\n"); exit(1);
		}
	}
}

void estasblishMultipleConns(struct sockaddr_in serverAddr){
	pthread_t tid;
	int sock;
	pthread_t arr[maxConns];
	int r=0;
	memset(arr,0,sizeof(arr));
	while(nLeftToConnect > 0){
		printf("nLeftToConnect = %d\n",nLeftToConnect );
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0){
			printf ("Error in opening a socket\n");
			exit (1);
		}
		// if(nonBlockingConnect(sock,serverAddr)==0){
		if(connect(sock,(struct sockaddr *)&serverAddr, sizeof(serverAddr))==0){
			printf("New connection established\n");
			nConns++;
			// spawn a thread to handle this new connection.
			int * arg=(int *)malloc(sizeof(int));
			*arg=sock;

			pthread_create(&tid, NULL, func,(void *)arg);
			arr[r]=tid;
			r++;
			// pthread_join(tid,NULL);
			// join it to main
		}
		nLeftToConnect--;
	}
	for(int i=0;i<r;i++){
		printf("JOINING %lld\n",(long long int)arr[i]);
		pthread_join(arr[i],NULL);
	}
	printf("%d Connections established\n",nConns );
}



int main(){

	int n=0;
	// fp=fopen("thetextsmall","r");
	// fd_set rfds,wfds;
	nLeftToConnect=maxConns;
	// clock_t programStart, programEnd;
	// clock_t requestStart, requestEnd;
	struct timeval tv1,tv2;
	double programTime;
		

	// sock = socket(AF_INET, SOCK_STREAM, 0);
	
	printf ("Client Socket Created\n");

	struct sockaddr_in serverAddr;
	memset (&serverAddr,0,sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT); 
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	printf ("Address assigned\n");

	
	// int clientSocket = connect (sock, (struct sockaddr*)&serverAddr,sizeof(serverAddr));
	// if (clientSocket < 0){
	// 	printf ("Error while establishing connection\n");
	// 	exit (0);
	// }
	// printf ("Connection Established\n");
		
	// nonBlockingConnect(serverAddr);
	gettimeofday(&tv1, NULL);
	estasblishMultipleConns(serverAddr);
	gettimeofday(&tv2, NULL);
	// func();

	programTime=(double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec)/1000000;
	
	printf("Time taken by the program = %f\n",programTime );
	printf("Time taken per request = %f\n",programTime/(double)numberOfRequests );
	printf("Throughtput =%f\n",(double)i/programTime);
	

	return 0;
}