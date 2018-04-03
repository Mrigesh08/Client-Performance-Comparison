#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>

#define MAXPENDING 5
#define BUFSIZE 1000
#define PORT 3000

void capitalize(char * buf, int x){
	for(int i=0;i<x;i++){
		buf[i] = (char)(buf[i] -32);
	}
	// return buf;
}

int main(){

	int serverSocket;
	struct sockaddr_in serverAddress, clientAddress;
	int bind_add;
	int clientLength;


	//creating SERVER SOCKET
	serverSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0){
		printf ("Error while server socket creation\n");
		exit (0);
	}
	printf ("Server Socket Created\n");


	//setting the address structure for SERVER
	memset (&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY
	printf ("Server address assigned\n");

	//binding SERVER to address
	bind_add = bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
	if (bind_add < 0){
		printf ("Error while binding\n");
		exit (0);
	}
	printf ("Binding successful\n");

	//listen call for SERVER
	int temp1 = listen(serverSocket, MAXPENDING);
	if (temp1 < 0){
		printf ("Error in listen\n");
		exit (0);
	}
	printf ("Now Listening\n");

	clientLength = sizeof(clientAddress);


	//****************server code*******************

	while(1){

		//accept call for all concurrent CLIENTS
		int clientSocket = accept (serverSocket, (struct sockaddr*) &clientAddress, &clientLength);
		if(clientSocket < 0){
			// printf ("Error in accept\n");
			exit(1);
		}
		printf("connection established with a client\n");


		//creating concurrent SERVER  by forking
		int childPid;
		if((childPid = fork())<0){
			printf ("Error in creating child process\n");
		}
		else if(childPid==0){
			close(serverSocket);

			char buffer[BUFSIZE];
			while(1){
				memset(buffer ,0, sizeof(buffer));
				int x=recv(clientSocket,buffer, sizeof(buffer),0);
				if(x==0){
					break;
				}
				capitalize(buffer,x);
				printf("%s\n", buffer);
				int y =send(clientSocket, buffer, sizeof(buffer),0);

			}
			close(clientSocket);
			printf("\n\nCLOSING CONNECTION\n\n");
			break;
		}
		close(clientSocket);
		
	}
	close(serverSocket);
	return 0;
}