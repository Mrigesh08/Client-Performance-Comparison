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

#define BUFSIZE 1000
#define PORT 3000
#define MAXLOOP 10


long long int i=0;
long long int i2=0;
long long int j=100000000; 
char sendbuf[BUFSIZE];
char recvbuf[BUFSIZE];
FILE * fp;
int sock;

void * func(){

	// while(fgets(sendbuf,1000,fp)!=NULL && feof(fp)==0){
	while(fread(sendbuf,1,1000,fp)>0){
		write(sock, sendbuf, sizeof(sendbuf));
		i+=1000;
		printf("SENT : %lld bytes till now :%s\n",i, sendbuf);
	}
	shutdown(sock,SHUT_WR);
	printf("\n\n\n\n\n\n\n\n Sending Thread closed \n\n\n\n\n\n\n\n\n\n");
	return (NULL);
}

void receive(){
	pthread_t tid;
	pthread_create(&tid, NULL, func, NULL);
	
	while(read(sock, recvbuf,sizeof(recvbuf)) > 0	){
		i2+=1000;
		printf("RECV : %lld bytes till now :%s\n", i2,recvbuf);
	}

	pthread_join(tid,NULL);

}


int main(){
	
	int n=0;
	fp=fopen("thetext","r");
	 // set it to any random value
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
	
	receive();

	close(sock);
	fclose(fp);
	programEnd = clock();
	programTime = (double)( programEnd -programStart )/(double)CLOCKS_PER_SEC;
	
	printf("Time taken by the program = %f\n",programTime );
	// printf("Time taken per request = %f\n",totalRequestTime/(double)numberOfRequests );
	printf("Throughtput =%f\n",(double)i/programTime);
	

	return 0;
}