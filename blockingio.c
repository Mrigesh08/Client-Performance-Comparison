#include <stdio.h>
#include <sys/socket.h> //for socket(), connect(), send(), recv() functions
#include <arpa/inet.h>// different address structures are declared here
#include <stdlib.h> // atoi() which convert string to integer
#include <string.h>
#include <unistd.h> // close() function
#include <errno.h>
#include <time.h>
#include <sys/time.h>


#define BUFSIZE 1000
#define PORT 3000
char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];
FILE * fp;

int main(){

	int n=0;
	fp=fopen("thetext","r");
	fd_set rfds,wfds;
	// clock_t programStart, programEnd;
	// clock_t requestStart, requestEnd;
	struct timeval tv1,tv2;
	double programTime;
	long long int numberOfRequests=0;	

	

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
	// long long int j=1000000000;
	// programStart = clock();
	gettimeofday(&tv1,NULL);
	while(1){
		printf("i=%lld \n",i );
		FD_SET(sock, &wfds);
		FD_SET(sock, &rfds);
		int x=select(sock+1,&rfds, &wfds, NULL, NULL);
		// send
		if(FD_ISSET(sock,&wfds)){
			if(fread(sendbuf,1,1000,fp)>0){
				int g=send(sock,sendbuf,sizeof(sendbuf),0);
				if(g==1000){
					i+=1000;

				}
				else{
					perror("send");
					exit(1);
				}
			}
			else{
				shutdown(sock, SHUT_WR);
				break;
			}
			printf("SENT : %s \n",sendbuf );
			numberOfRequests++;
			
		}
		if(FD_ISSET(sock,&rfds)){
			int g=recv(sock,recvbuf, sizeof(recvbuf),0 );
			if(g<=0){
				break;
			}
			printf("RECV : %s\n", recvbuf);
		}
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
	}
	
	close(sock);

	// programEnd = clock();
	gettimeofday(&tv2,NULL);
	// programTime = (double)( programEnd -programStart )/(double)CLOCKS_PER_SEC;
	programTime=(double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec)/1000000;
	printf("Time taken by the program = %f\n",programTime );
	printf("Time taken per request = %f\n",programTime/(double)numberOfRequests );
	printf("Throughtput =%f\n",(double)i/programTime);
	

	return 0;
}