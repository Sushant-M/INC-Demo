CC = gcc
main: main.c
	$(CC) -o main main.c confighandlers.c raft.c -lconfig -lsocket -lpthread
