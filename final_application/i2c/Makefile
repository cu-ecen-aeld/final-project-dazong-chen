CC=gcc
CROSS_COMPILE=

all: main

main: main.o
	$(CROSS_COMPILE)$(CC) -o main main.o 
	
main.o: main.c
	$(CROSS_COMPILE)$(CC) -c main.c
    
clean: 
	rm -f main *.o

