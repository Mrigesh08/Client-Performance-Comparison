#include <stdio.h>
#include <sys/socket.h> //for socket(), connect(), send(), recv() functions
#include <arpa/inet.h>// different address structures are declared here
#include <stdlib.h> // atoi() which convert string to integer
#include <string.h>
#include <unistd.h> // close() function
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>

#define BUFSIZE 1024
#define PORT 3000
#define MAXLOOP 10

long long int i=0;
long long int j=100000000; 
char buf[BUFSIZE];
int sock;
// creating mutex lock;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * func(){
	while(i<j){
		int g=send(sock,buf,sizeof(buf),0);
		if(g==1024){ 
			pthread_mutex_lock(&mutex);
			printf("%lld : Number of bytes sent till now = %lld\n",(long long int)pthread_self(),i );
			i = i + 1024;
			pthread_mutex_unlock(&mutex);
		}	
	}
	
}

int main(){
	
	int n=0;
	
	memset(buf, 65, sizeof(buf)); // set it to any random value
	buf[1023]='\0';
	fd_set rfds,wfds;
	clock_t programStart, programEnd;
	clock_t requestStart, requestEnd;
	double requestTime, programTime;
	double totalRequestTime=0;
	long long int numberOfRequests=0;	

	
	programStart = clock();

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		printf ("Error in opening a socket\n");
		exit (0);
	}
	printf ("Client Socket Created\n");

	struct sockaddr_in serverAddr;
	memset (&serverAddr,0,sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT); 
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	printf ("Address assigned\n");

	int clientSocket = connect (sock, (struct sockaddr*)&serverAddr,sizeof(serverAddr));
	if (clientSocket < 0){
		printf ("Error while establishing connection\n");
		exit (0);
	}
	printf("Connection Established\n");
	
	
	// Creating Threads;
	pthread_t tidA, tidB;
	pthread_create(&tidA, NULL, &func, NULL);
	pthread_create(&tidB, NULL, &func, NULL);




	pthread_join(tidA, NULL);
	pthread_join(tidB, NULL);

	close(sock);

	programEnd = clock();
	programTime = (double)( programEnd -programStart )/(double)CLOCKS_PER_SEC;
	
	printf("Time taken by the program = %f\n",programTime );
	// printf("Time taken per request = %f\n",totalRequestTime/(double)numberOfRequests );
	printf("Throughtput =%f\n",(double)i/programTime);
	

	return 0;
}