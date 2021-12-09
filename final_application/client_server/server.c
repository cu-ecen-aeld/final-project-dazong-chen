/***********************************************************
* File name: server.c
* Description: temperature sensor reads the temperature value, passes it to server and server sends the value as string to client
* Name: Dazong Chen
* Reference: https://beej.us/guide/bgnet/html/#socketman
***********************************************************/
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

#define		PORT			"3490"  	// the port users will be connecting to
#define 	BACKLOG 		10	 	// how many pending connections queue will hold
#define		MAX_DATA_BYTES		100
//#define		MAX_TEMP_DATA_BYTES	10

#define 	TEMP_SENSOR_ADDR			0x40
#define		I2C_DEVICE_FILE          		"/dev/i2c-2"
#define		TEMPERATURE_MEASUREMENT_NO_HOLD		0xF3
#define		STATUS_BITS				0x03     // The two status bits, must be set to 0 before calculating physical values
#define		TEMP_DATA_BYTES				2

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

float read_temperature();

int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	float 		temp = 0;	// temparature data
	char 		data_buf[MAX_DATA_BYTES];	// the string that server sends to client
	//char		temp_data_buf[MAX_TEMP_DATA_BYTES];	// string that contains the value of temperature
	
	while(1) 
	{  	// main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) 
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		
		while(1)
		{
			memset(data_buf, 0, MAX_DATA_BYTES);
			
			
			//strcpy(data_buf, "server send: ");
			
			temp = read_temperature();
			
			sprintf(data_buf, "%.2f", temp);
			
			int total_bytes = strlen(data_buf)+1;
			int bytes_sent = 0;
			
			do
			{
				bytes_sent = send(new_fd, data_buf, total_bytes, 0);
				if (bytes_sent == -1)
				{
					perror("send");
					close(new_fd);
					exit(-1);
				}
				
				total_bytes -= bytes_sent;
				
			}while(total_bytes != 0);
			sleep(2);
		}
	}

	return 0;
}

float read_temperature()
{
	int file = open(I2C_DEVICE_FILE, O_RDWR);
	int i2c_addr = 0;
	
	if(file < 0)
	{
		syslog(LOG_ERR, "failed to open i2c-2\n");
		printf("failed to open i2c-2\n");
	}
	
	i2c_addr = TEMP_SENSOR_ADDR;
	
	// failed to specify with device address
	if(ioctl(file, I2C_SLAVE, i2c_addr) < 0)
	{
		syslog(LOG_ERR, "failed to specify with device address");
		printf("failed to specify with device address\n");
	}

	char buf[10] = {0};
	
	buf[0] = TEMPERATURE_MEASUREMENT_NO_HOLD;
	
	write(file, buf, 1);
	
	usleep(60000);	// measurement time
	
	char data_buf[TEMP_DATA_BYTES] = {0};
	int rc = read(file, data_buf, TEMP_DATA_BYTES);
	
	if( rc != 2)
	{
		syslog(LOG_ERR, "i2c read transaction failed %d\n",rc);
		printf("i2c read transaction failed %d\n",rc);
	}
	
	// calculate temperature value
	uint16_t	val = 0;
	float		temp = 0;
	
	val = ( (data_buf[0] << 8 | data_buf[1]) &~ STATUS_BITS);
	temp = -46.85 + (175.72*val/65536);
	
	return 	temp;
}

