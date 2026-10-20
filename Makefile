CC = gcc
CFLAGS = -Wall -Wextra -g

all: client server

client:
	$(CC) $(CFLAGS) -o client/main client/main.c

server:
	$(CC) $(CFLAGS) -o server/main server/main.c

clean:
	rm -f client/main server/main

re: clean all

.PHONY: all client server clean re