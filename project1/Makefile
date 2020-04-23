#Makefile

CC = g++
CFLAGS = -g

server: server.o
	$(CC) -o $@ $^ $(CFLAGS)

server.o : server.cc
	$(CC) -c -o $@ $^ $(CFLAGS)