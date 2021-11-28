/********************************************************
* File Name: main.c
* Description: application entry point
* Author: Dazong Chen
* Date: 2021.11.24
* Reference: https://www.kernel.org/doc/Documentation/i2c/dev-interface
********************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>


#define 	GESTURE_SENSOR_ADDR 			0x39
#define 	TEMP_SENSOR_ADDR			0x40
#define		I2C_DEVICE_FILE          		"/dev/i2c-2"
#define		TEMPERATURE_MEASUREMENT_NO_HOLD		0xF3
#define		STATUS_BITS				0x03     // The two status bits, must be set to 0 before calculating physical values



int main()
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
	
	sleep(1);
	
	char data_buf[2] = {0};
	int rc = read(file, data_buf, 2);
	
	if( rc != 2)
	{
		syslog(LOG_ERR, "i2c read transaction failed %d\n",rc);
		printf("i2c read transaction failed %d\n",rc);
	}
	
	// calculate temperature value
	uint16_t        val = 0;
	float       temp = 0;
	val = (data_buf[0] << 8 | data_buf[1] &~ STATUS_BITS);
	temp = (125*val/65536) - 6;
	
	printf("temperature = %.2f C\n", temp);
	
	return 0;
}
