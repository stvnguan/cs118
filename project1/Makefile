#Makefile

CC = g++
CFLAGS = -g

server.o : server.cc
	$(CC) -c -o $@ $^ $(CFLAGS)

server: server.o
	$(CC) -o $@ $^ $(CFLAGS)