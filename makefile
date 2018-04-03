all:
	gcc twothreads.c -lpthread -o twothreads
	gcc blockingio.c -o blockingio
	gcc twoprocesses.c -o twoprocesses
	gcc server.c -o server