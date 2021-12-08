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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define     BBB_GPIO_DIRECTORY "/sys/class/gpio"
#define     MAX_BUF 64
#define     GPIO_INPUT  (0)
#define     GPIO_OUTPUT (1)
#define     LED_COLD (48) //9_15
#define     LED_HOT (49) //9_23
#define     PORT "3490" // the port client will be connecting to 
#define     MAXDATASIZE 100 // max number of bytes we can get at once
#define		RECV_FAIL 	-2
#define		OVER_FLOW	-3
#define		RECV_SUCCESS	0

int gpio_init(unsigned int gpio_num, unsigned int direction)
{
    	int fd_exp, len_exp;
    	int fd_dir;
    	char buf_exp[MAX_BUF];
    	char buf_dir[MAX_BUF];

    	fd_exp = open(BBB_GPIO_DIRECTORY "/export", O_WRONLY);
    	if (fd_exp < 0) 
    	{
        	perror("gpio/export");
        	return fd_exp;
    	}

	len_exp = snprintf(buf_exp, sizeof(buf_exp), "%d", gpio_num);
	write(fd_exp, buf_exp, len_exp);
	close(fd_exp);

	snprintf(buf_dir, sizeof(buf_dir), BBB_GPIO_DIRECTORY "/gpio%d/direction", gpio_num);
	fd_dir = open(buf_dir, O_WRONLY);
	if (fd_dir < 0) 
    	{
		perror("ERROR: Failed to initialize gpio and set direction");
		return fd_dir;
	}

	if (direction)
	write(fd_dir, "out", 4);
	else
	write(fd_dir, "in", 3);

	close(fd_dir);
	return 0;
}


int gpio_deinit(unsigned int gpio_num)
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(BBB_GPIO_DIRECTORY "/unexport", O_WRONLY);
    if (fd < 0) 
    {
        perror("ERROR: Failed to deinitialize gpio ");
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio_num);
    write(fd, buf, len);
    close(fd);
    return 0;
}


int gpio_set_value(unsigned int gpio_num, unsigned int value)
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), BBB_GPIO_DIRECTORY "/gpio%d/value", gpio_num);
    fd = open(buf, O_WRONLY);
    if (fd < 0) 
    {
        perror("ERROR: Failed to set gpio value");
        return fd;
    }

    if (value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

int gpio_get_value(unsigned int gpio_num, unsigned int *value)
{
    int fd;
    char buf[MAX_BUF];
    char ch;

    snprintf(buf, sizeof(buf), BBB_GPIO_DIRECTORY "/gpio%d/value", gpio_num);
    fd = open(buf, O_RDONLY);
    if (fd < 0) 
    {
        perror("ERROR: Failed to get gpio value ");
        return fd;
    }

    read(fd, &ch, 1);

    if (ch != '0') 
    {
        *value = 1;
    } else 
    {
        *value = 0;
    }

    close(fd);
    return 0;
}

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

    int ret = 0;
	if((ret = gpio_init(LED_COLD, GPIO_OUTPUT)) != 0)
	{
		perror("FAILED TO INIT LED");
		exit(EXIT_FAILURE);
	}
    if((ret = gpio_init(LED_HOT, GPIO_OUTPUT)) != 0)
	{
		perror("FAILED TO INIT LED");
		exit(EXIT_FAILURE);
	}

    if((ret = gpio_set_value(LED_COLD, 1)) != 0)
    {
        perror("FAILED TO SET LED TO 1");
        exit(EXIT_FAILURE);
    }
    if((ret = gpio_set_value(LED_HOT, 1)) != 0)
    {
        perror("FAILED TO SET LED TO 1");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    if((ret = gpio_set_value(LED_COLD, 0)) != 0)
    {
        perror("FAILED TO SET LED TO 0");
        exit(EXIT_FAILURE);
    }
    if((ret = gpio_set_value(LED_HOT, 0)) != 0)
    {
        perror("FAILED TO SET LED TO 0");
        exit(EXIT_FAILURE);
    }
    sleep(1);
	

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
	double temp;
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
			printf("temperature is %.2f\n", temp);
            if(temp > 24.0)
            {

                if((ret = gpio_set_value(LED_COLD, 0)) != 0)
                {
                    perror("FAILED TO SET LED TO 1");
                    exit(EXIT_FAILURE);
                }
                if((ret = gpio_set_value(LED_HOT, 1)) != 0)
                {
                    perror("FAILED TO SET LED TO 1");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if((ret = gpio_set_value(LED_COLD, 1)) != 0)
                {
                    perror("FAILED TO SET LED TO 0");
                    exit(EXIT_FAILURE);
                }
                if((ret = gpio_set_value(LED_HOT, 0)) != 0)
                {
                    perror("FAILED TO SET LED TO 0");
                    exit(EXIT_FAILURE);
                }
                sleep(1);
            }
		}
		
	}
	
	close(sockfd);

	return 0;
}
