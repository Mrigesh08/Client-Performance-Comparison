# Client-Performance-Comparison
Perfomance Comparison of different batch mode clients
		by – Mrigesh Madaan and Sanchit Khanna

NOTE1 : All the clients use a file called “thetext”. Data from this file is tranffered to server in case of  (a) to (d) and is tranffered to client in case of (e) anb (f). A program called “createFile.c” has been included which can be used to generate this file to be of any desired size.

NOTE2 : The times have been measured using a 1GB file. 10GB file was taking a lot of time to transfer in some cases, which is why we chose to tranfer only 1GB file. 

NOTE3: Part(e) and Part(f) of the questions require the use of the file server2.c

Part (a) : Blocking IO with Select
	The psuedocode for this part is mentioned below.  The psuedocode is pretty much self-explanatory.
  	
	main (){
		socket();
		connect();
		startDataTranferClock();		
		while(1){
			setFDs();
			select(sock+1, &rfds, &wfds, NULL, NULL);
			if( isReadable(sock) ){ 
				startResponseTimeClock();
				read(sock);
				stopResponseTimeClock();
			}
			if( isWritable(sock) ){ write(stdout);}
			clearFDs();
		}
		stopDataTransferClock();
	}

Part (b) : Non-blocking IO with Select
	This part uses non blocking IO for sending and receiving using select. The main crux behind  achieving this part is buffer management. Rest all is similar to what was done in the blocking IO part. This part involves two buffers (of 1KB) : toBuffer and fromBuffer. There are two pointers required to manage each of the buffers.
	ToBuffer is used to keep the data that is to be sent to the server. It has two pointers : “toiptr” and “tooptr”. Data that is present after the “tooptr” represents the data that is yet to be written to the socjet and sent to the server. Data after the “toiptr” is actually empty space meaning that data can be read into this buffer after this pointer. Whenever data is sent, tooptr is incremented by the number of bytes sent. Both the pointers are adjusted once the buffer has been completely sent to the server.
	FromBuffer is used to keep data that has come from the server. This data is then written to stdout. It is also managed by two pointers : friptr and froptr. Data after froptr is yet to be written to stdout and data after friptr is empty space and more data can be read into this space from the server.
	The psuedocode for this part is mentioned below. The time measuring calls have been ommitted to provide clarity
		
	

	main (){
		socket();
		connect();
		setNonBlocking(sock,file,stdout);		
		while(1){
			setFDs();
			select(sock+1, &rfds, &wfds, NULL, NULL);
			if(isReadable(file)){ 
				read(file, toBuffer, sizeof(toBuffer)); 
				manageBufferPointers();
			}				
			if( isWritable(sock) ){ 
				write(sock,toBuffer,sizeof(toBuffer));
				manageBufferPointers();
			}
			if( isReadable(sock) ){ 
				read(sock, fromBuffer, sizeof(fromBuffer));
				manageBufferPointers();
			}
			if(isWritable(stdout)){
				write(stdout, fromBuffer, sizeof(fromBuffer));
				manageBufferPointers();
			}
			
			clearFDs();
		}
	} 

Part (c) : Two Processes
	In this part, the client process forks a new child process. The parent and the child, both share the same socket connection. The child process is used to send the request to the server. It reads the text from a file and writes it to socket. The parent process is used to receive the replies from the server. It reads from the socket and wrote to stdout. Both the parent and child use blocking IO.

Part (d) : Two Threads
	In this part, the main thread spawns a new thread. The new thread is used to send data to the server while the main thread is used to receive data from the server. This part also makes use of blocking IO.

Required time measurements (averaged over three runs for 1GB file): Please note that the programs are taking a higher time to run becuase output is being printed on the terminal. If out is not printed, time taken by the program is significantly less(in the range of 15-20secs). 

Part (e) : Non Blocking IO with Non Blocking Connect (requires server2.c)
	In this part, the programs tries to connect to a server using a non-blocking connect and as soon as connection is established, a new Thread is spawned to receive data from the server. On the server side as soon as a new connection is established, the server starts sending data by reading from a file.
The following psuedocode illustrates the above.
	
	establishMultipleConnections(){
		nLeftToConnect = maxConnections;
		while( nLeftToConnect > 0 ){
			sock = socket();
			if( nonBlockingConnect(sock) == true){
				arg=sock;
				pthread_create(&tid, NULL, func, arg);
				// “func” is a function that is to receive data from the server using  				//  non- blocking IO
				// This non-blocking IO is the same as that in part (b)
			}
			nLeftToConnect--;
		}
	}
  
  Part (f) : Blocking IO with Threads (requires server2.c)
	This part is essentially the same as the previous part. The difference is that we are blocking IO in this part instead of using non-blocking IO. The psuedocode for this part is as follows.
	
	establishMultipleConnections(){
		nLeftToConnect = maxConnections;
		while( nLeftToConnect > 0 ){
			sock = socket();
			if( connect(sock) == true){ // changed here
				arg=sock;
				pthread_create(&tid, NULL, func, arg); // changed here
				// “func” is a function that is to receive data from the server
				// “func” in this case employs blocking IO
			}
			nLeftToConnect--;
		}
	}

