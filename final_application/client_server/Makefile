CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif


all: client server
default: client server

client : client.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) client.c -o client

server : server.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) server.c -o server

clean :
	rm -f client server *.o
