CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

all: client server
default: client server

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

client: client.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) client.o -o client

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

server: server.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) server.o -o server

clean:
	rm -f client server *.o
