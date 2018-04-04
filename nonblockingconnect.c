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

#define BUFSIZE 1000
#define PORT 3000
// char recvbuf[BUFSIZE];
// char sendbuf[BUFSIZE];
long long int i=0;
long long int i2=0;
FILE * fp;
// int sock;
int maxConns=1;
int nConns=0;
int nLeftToConnect;
int nsecs=2;

int max(int a, int b){
	return a>b?a:b;
}

int nonBlockingConnect(int sock,struct sockaddr_in serverAddr){
	int flags, n , error;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;
	flags=fcntl(sock, F_GETFL,0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	error =0;
	
	if(n=connect(sock,(struct sockaddr*)&serverAddr,sizeof(serverAddr))){
		if(errno!=EINPROGRESS)
		{ printf("SOME OTHER ERROR %d\n",errno);	return -1;}
	}
	if(n==0){
		printf("STRAIGHT JUMP\n");
		goto done;
	}
	else{
		printf("CONTINUING\n");
	}

	FD_ZERO(&rset);
	FD_SET(sock, &rset);
	wset=rset;
	tval.tv_sec = nsecs;
	tval.tv_usec = 0;

	if((n= select(sock +1 , &rset, &wset,NULL,&tval))==0){
		errno=ETIMEDOUT;
		printf("TIMEOUT\n");
		return -1;
	}
	if(FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset)){
		len=sizeof(error);
		if(getsockopt(sock,SOL_SOCKET,SO_ERROR,&error,&len)<0)
		{ printf("getsockopt ERROR\n");	return -1;}
	}
	else{
		printf("select error. sock was not set.\n");
	}

	done:
		fcntl(sock,F_SETFL,flags);
		if(error){
			close(sock);
			printf("ERROR occured\n");
			exit(1);
		}

	return 0;
}



void * func(void * arg){
	// declare variables
	int sock=*((int *)arg);
	int maxfd, val, stdineof;
	int n, nwritten ,n2;
	fd_set rfds, wfds;
	char to[BUFSIZE], from[BUFSIZE];
	char *toiptr, *tooptr, *friptr, *froptr;

	// make socket and IO non blocking
	val = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, val | O_NONBLOCK);
	val = fcntl(fileno(fp),F_GETFL,0);
	fcntl(fileno(fp), F_SETFL, val | O_NONBLOCK);
	val = fcntl(STDOUT_FILENO,F_GETFL,0);
	fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);


	toiptr = tooptr = to;
	froptr = friptr = from;
	stdineof =0 ;
	maxfd = max(max(STDOUT_FILENO, fileno(fp)),sock);
	

	while(1){
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		if(stdineof == 0 && (toiptr < &to[BUFSIZE]))
			FD_SET(fileno(fp), &rfds);
		if(friptr < &from[BUFSIZE])
			FD_SET(sock, &rfds);
		if(tooptr != toiptr)
			FD_SET(sock, &wfds);
		if(froptr != friptr)
			FD_SET(STDOUT_FILENO, &wfds);


		int g=select(maxfd+1, &rfds, &wfds, NULL, NULL);
		if(g==-1){perror("select"); exit(1); }


		// Reading from file
		if(FD_ISSET(fileno(fp), &rfds)){
			if((n=read(fileno(fp) , toiptr , &to[BUFSIZE] - toiptr)) < 0){
				if(errno != EWOULDBLOCK){ perror("read");  exit(1);}
			}
			else if(n==0){
				printf("\n\nALL DATA SENT\n\n" );
				stdineof =1;
				if(tooptr == toiptr)
					shutdown(sock, SHUT_WR); // shutdown the write end of the socket
			}
			else{
				toiptr+=n;
				// printf("%d Bytes read from the file\n", n );
				FD_SET(sock,&wfds);
			}
		}
		

		// Writing to Socket
		if(FD_ISSET(sock, &wfds) && (n > 0)){
			// printf("WRITING TO SOCKET\n");
			if((nwritten = write(sock,tooptr,n) )< 0){
				if(errno != EWOULDBLOCK){ printf("write error 1\n");close(sock);  return NULL;}
			}
			else{
				i+=nwritten;
				// printf("SENT : %lld Total bytes sent till now. %s\n",i,tooptr);
				tooptr+=nwritten;
				if(tooptr == toiptr){
					// printf("RESETTING\n");
					toiptr = tooptr = to;
					if(stdineof)
						shutdown(sock,SHUT_WR);
				}
				else{
					n=n-nwritten; // so the next time, only the remaining n characters will be written
				}
			}
		}
		// else{
		// 	printf("SOCKET NOT WRITABLE\n");
		// 	printf("%d\n",FD_ISSET(sock, &wfds) );
		// 	printf("%d\n",n);
		// }

		
		// Reading from the socket
		if(FD_ISSET(sock, &rfds)){
			// printf("READING THE REPLY FROM THE SERVER\n");
			if((n2=read(sock , friptr , &from[BUFSIZE] - friptr)) < 0){
				if(errno != EWOULDBLOCK){ perror("read");  exit(1);}
			}
			else if(n2==0){
				printf("\n\nALL DATA READ\n\n" );
				if(stdineof ==1){ close(sock); return NULL ;}
				else printf("ERROR : server terminated prematurely.\n");
				if(tooptr == toiptr)
					shutdown(sock, SHUT_WR); // shutdown the write end of the socket
			}
			else{
				friptr+=n2;
				// printf("%d Bytes read from the socket\n", n2 );
				FD_SET(STDOUT_FILENO, &wfds); 
			}
		}

		
		// Writing server reply to STDOUT
		if(FD_ISSET(STDOUT_FILENO, &wfds) && n2 > 0){
			// printf("WRITING TO STDOUT\n");
			if((nwritten = write(STDOUT_FILENO,froptr,n2) )< 0){
				if(errno != EWOULDBLOCK){ printf("write error 2\n");  exit(1);}
			}
			else{
				i2+=nwritten;
				// printf("\nRECV : %lld Total bytes received till now.\n",i2);
				froptr+=nwritten;
				if(froptr == friptr){
					// printf("RESETTING2\n");
					friptr = froptr = from;
				}
				else{
					n2=n2-nwritten;
				}
			}
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
		if(nonBlockingConnect(sock,serverAddr)==0){
			printf("New connection established\n");
			nConns++;
			// spawn a thread to handle this new connection.
			int * arg=(int *)malloc(sizeof(int));
			*arg=sock;
			
			pthread_create(&tid, NULL, func,(void *)arg);
			arr[r]=tid;
			r++;
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
	fp=fopen("thetext","r");
	// fd_set rfds,wfds;
	nLeftToConnect=maxConns;
	clock_t programStart, programEnd;
	clock_t requestStart, requestEnd;
	double requestTime, programTime;
	double totalRequestTime=0;
	long long int numberOfRequests=0;	

	programStart = clock();

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
	estasblishMultipleConns(serverAddr);
	// func();

	programEnd = clock();
	programTime = (double)( programEnd -programStart )/(double)CLOCKS_PER_SEC;
	
	printf("Time taken by the program = %f\n",programTime );
	printf("Time taken per request = %f\n",totalRequestTime/(double)numberOfRequests );
	printf("Throughtput =%f\n",(double)i/programTime);
	

	return 0;
}