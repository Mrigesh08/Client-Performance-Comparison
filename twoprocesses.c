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

#define BUFSIZE 1024
#define PORT 3000

long long int * i;
long long int j=1000000000; 



union semun{
	int val;
};

int main(){

	struct sembuf sops1[1];
	sops1[0].sem_num = 0;
	sops1[0].sem_op = -1;
	sops1[0].sem_flg = 0;

	struct sembuf sops2[1];
	sops2[0].sem_num = 0;
	sops2[0].sem_op = 1;
	sops2[0].sem_flg = 0;
	
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
	printf("Connection Established\n");

	// semaphore initializations;
	union semun arg;
	arg.val=1;
	int semid=semget(54874, 1, IPC_CREAT | 0666);
	if(semid==-1){	perror("semget"); exit(1);	}
	if(semctl(semid,0,SETVAL,arg)==-1){	perror("semctl"); exit(1);}

	// shared memory initializations;
	int shmid=shmget(21543, sizeof(long long int), IPC_CREAT | 0666);
	if(shmid==-1){ perror("shmid"); exit(1); }
	i=(long long int *)shmat(shmid, NULL, 0 );
	if(i == (long long int *)-1){ perror("shmat"); exit(1);	}

	int child=fork();
	if(child<0){ perror("fork"); exit(1); }
	else if(child==0){
		// child 
		semid = semget(54874, 1, 0666);
		if(semid==-1){ perror("semid"); exit(1); }
		shmid = shmget(21543, sizeof(long long int), 0666);
		if(shmid==-1){ perror("shmid"); exit(1); }
		i=(long long int *)shmat(shmid, NULL, 0 );
		if(i == (long long int *)-1){ perror("shmat"); exit(1);	}

		int s;
		while(*i<j){
			
			// acquire sem
			// send and increment the value of i
			// relase sem
			
			s=semop(semid, sops1 , 1);
			if(s==-1){perror("semop"); exit(1); }

			printf("Process 2 : i = %lld\n",*i );
			// requestStart= clock();
			int g=send(sock,buf,sizeof(buf),0);
			if(g==1024){ *i = *i + 1024;}

			s=semop(semid, sops2 , 1);
			if(s==-1){perror("semop"); exit(1); }

		}
	}
	else{
		// Parent
		int s;
		while(*i<j){

			s=semop(semid, sops1 , 1);
			if(s==-1){perror("semop"); exit(1); }

			printf("Process 1 : i = %lld\n",*i );
			int g=send(sock,buf,sizeof(buf),0);
			if(g==1024){ *i = *i + 1024;}

			s=semop(semid, sops2 , 1);
			if(s==-1){perror("semop"); exit(1); }
		}

		// printf("Time taken per request = %f\n",totalRequestTime/(double)numberOfRequests );	
	}
	close(sock);

	programEnd = clock();
	programTime = (double)( programEnd -programStart )/(double)CLOCKS_PER_SEC;
	
	printf("Time taken by the program = %f\n",programTime );
	// printf("Time taken per request = %f\n",totalRequestTime/(double)numberOfRequests );
	printf("Throughtput =%f\n",(double)*i/programTime);
	

	return 0;
}