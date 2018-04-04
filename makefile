all:
	gcc twothreads.c -lpthread -o twothreads
	gcc blockingio.c -o blockingio
	gcc nonblockingio.c -o nonblockingio
	gcc nonblockingconnect.c -lpthread -o nonblockingconnect
	gcc server2.c -o server2
	gcc twoprocesses.c -o twoprocesses
	gcc server.c -o server
	gcc createFile.c -o createFile