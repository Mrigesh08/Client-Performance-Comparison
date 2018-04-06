#include <stdio.h>
#include <sys/socket.h> //for socket(), connect(), send(), recv() functions
#include <arpa/inet.h>// different address structures are declared here
#include <stdlib.h> // atoi() which convert string to integer
#include <string.h>
#include <unistd.h> // close() function
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define BUFSIZE 1000
#define PORT 3000

long long int i=0;
long long int i2=0;
long long int j=1000000000; 
char sendbuf[BUFSIZE];
char recvbuf[BUFSIZE];
FILE * fp;
int sock;


int main(){
	
	int n=0;
	fp=fopen("thetext","r");
	// clock_t programStart, programEnd;
	// clock_t requestStart, requestEnd;
	struct timeval tv1,tv2;
	double programTime;
	long long int numberOfRequests=0;		

	

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

	// programStart = clock();
	gettimeofday(&tv1,NULL);

	int child=fork();
	if(child<0){ perror("fork"); exit(1); }
	else if(child==0){
		// child  
		// used to send data
	
		while(fread(sendbuf,1,1000,fp)>0){
			write(sock, sendbuf, sizeof(sendbuf));
			i+=1000;
			numberOfRequests++;
			printf("SENT : %lld bytes till now :%s\n",i, sendbuf);
		}
		shutdown(sock,SHUT_WR);
		printf("\n\n\n\n\n\n\n\n Sending Process closed \n\n\n\n\n\n\n\n\n\n");

		exit(0);	
	}
	else{
		// Parent
		// used to receive data
		while(read(sock, recvbuf,sizeof(recvbuf)) > 0	){
			i2+=1000;
			printf("RECV : %lld bytes till now :%s\n", i2,recvbuf);
		}
		// programEnd = clock();
		gettimeofday(&tv2,NULL);
		programTime=(double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec)/1000000;
		
		printf("Time taken by the program = %f\n",programTime );
		printf("Time taken per request = %f\n",programTime/(double)numberOfRequests );
		printf("Throughtput =%f bytes/s\n",(double)i2/programTime);
		
		close(clientSocket);
		close(sock);

	}
	
	return 0;
}