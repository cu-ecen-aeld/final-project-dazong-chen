CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

HFILES= htu21d_sensor/htu21d_sensor.h  

all: client server
default: client server

client : client_server/client.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) client_server/client.c -o client

server : htu21d_sensor/htu21d_sensor.c client_server/server.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) client_server/server.c -o server

clean :
	rm -f client server *.o
