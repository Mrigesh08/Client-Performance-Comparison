#include <stdio.h>
#include <sys/socket.h> //for socket(), connect(), send(), recv() functions
#include <arpa/inet.h>// different address structures are declared here
#include <stdlib.h> // atoi() which convert string to integer
#include <string.h>
#include <unistd.h> // close() function
#include <errno.h>
#include <time.h>

#define BUFSIZE 1024
#define PORT 3000


int main(){

	int n=0;
	char buf[BUFSIZE];
	memset(buf, 65, sizeof(buf)); // set it to any random value
	buf[1023]='\0';
	fd_set rfds,wfds;
	clock_t programStart, programEnd;
	clock_t requestStart, requestEnd;
	double requestTime, programTime;
	double totalRequestTime=0;
	long long int numberOfRequests=0;	

	programStart = clock();

	int sock = socket(AF_INET, SOCK_STREAM, 0);
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
	printf ("Connection Established\n");

	long long int i=0;
	long long int j=1000000000;
	while(i<j){
		printf("i=%lld \n",i );
		FD_SET(sock, &wfds);
		int x=select(sock+1,NULL, &wfds, NULL, NULL);
		// send
		if(FD_ISSET(sock,&wfds)){
			requestStart = clock();
			int g=send(sock,buf,sizeof(buf),0);
			requestEnd = clock();
			requestTime=(double)(requestEnd-requestStart)/(double)CLOCKS_PER_SEC;
			printf("RequestTime = %f\n",requestTime );
			totalRequestTime+=requestTime;
			numberOfRequests++;
			if(g==1024){
				i+=1024;
			}
			else{
				perror("send");
				exit(1);
			}
		}
		FD_ZERO(&wfds);
	}
	
	close(sock);

	programEnd = clock();
	programTime = (double)( programEnd -programStart )/(double)CLOCKS_PER_SEC;
	
	printf("Time taken by the program = %f\n",programTime );
	printf("Time taken per request = %f\n",totalRequestTime/(double)numberOfRequests );
	printf("Throughtput =%f\n",(double)i/programTime);
	

	return 0;
}