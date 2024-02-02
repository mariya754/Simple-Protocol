
CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS =

all: server client

server: server.o 
	$(CC) $(LDFLAGS) -o server server.o factor.o

client: client.o factor.o
	$(CC) $(LDFLAGS) -o client client.o factor.o

server.o: server.c factor.o
	$(CC) $(CFLAGS) -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

factor.o: factor.c factor.h
	$(CC) $(CFLAGS) -c factor.c

clean:
	rm -f server client *.o