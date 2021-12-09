/***********************************************************
* File name: client.c
* Description: client receives the string that server sends and changes PWM value accordingly
* Name: Chris Choi
* Reference: https://beej.us/guide/bgnet/html/#socketman
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define		RECV_FAIL 	-2
#define		OVER_FLOW	-3
#define		RECV_SUCCESS	0

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	char* ptr = NULL;
	int rc = 0;
	double temp;	// temperature value
	
	while(1)
	{
		ptr = buf;
		
		while(1)
		{	
			
			if ((recv(sockfd, ptr, 1, 0)) == -1) 
			{
		    		perror("recv");
		    		close(sockfd);
		    		exit(-1);
		    		rc = RECV_FAIL;
			}
			
			
			if(*ptr == '\0')
			{
				// end of string
				rc = RECV_SUCCESS;
				break;	
			}
			
			else if( (ptr - buf) >= (MAXDATASIZE-1) )
			{
				// buf size exceeded
				printf("buf size exceeded\n");
				rc = OVER_FLOW;
				break;
			}
			
			else
			{
				ptr++;
			}
		}
		
		if(rc == RECV_SUCCESS)
		{	
			temp = atof(buf);
			printf("temperature is %.2f C\n",temp);
		}
		
	}
	
	close(sockfd);

	return 0;
}

