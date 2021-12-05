CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

#HFILES= htu21d_sensor/htu21d_sensor.h

all: client server
default: client server

client : client.o
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o client client.o

server : client_server/server.c htu21d_sensor/htu21d_sensor.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o server client_server/server.c htu21d_sensor/htu21d_sensor.c -I.

client.o: client_server/client.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c client_server/client.c

clean :
	rm -f client server *.o
