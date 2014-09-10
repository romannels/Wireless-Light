/***************************************************************
 * 			PWM Dimmer			       *
 * 	      	       Roman Nelson			       *
 * 	      	    Voss Lighting 2014			       *
 * 	      	                                               *
 * Accepts user input from a serial console to manipulate      *
 * PWM perepherial 0A on the AM3359.                           *
 * ************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include "PWM_Enabler.h"

/* PROTOTYPES */
void change_duty_cycle(int level);
void PWM_run(char *light_switch);
void change_period(long period);
void respond_to_command(char *read_buffer, int chars_read);
void serial_port_close(void);
int serial_port_open(void);
int serial_port_read(char *read_buffer, size_t max_chars_to_read);
void serial_port_write(char *write_buffer);
void sigint_handler(int sig);

static const char *PORT_NAME = "/dev/ttyUSB0"; // USB0 location

int serial_port;
struct termios options_original; // struct for serial communication




int main()
{
	int chars_read;
	char read_buffer[MAX_COMMAND_LENGTH +1] = {0}; // termiinal input buffer
	
	enable_PWM(); // Turn on EHRPWM0A
	serial_port_open();
	printf("after serial_port_open()\n");
	if(serial_port != -1)
	{
		signal(SIGINT, (void*)sigint_handler); // listen for special commands

		serial_port_write("USB virtual serial port test program\r\n");
		serial_port_write("Please type changeperiod, changedutycycle, on, or off\r\n");
		serial_port_write("To close the application, type closeapp and press <Enter>\r\n");	
	}
	else {printf("something is wrong with serial port\n");}
	for(;;)
	{
	//	printf("in loop");
		if(serial_port != -1)
		{
		//	printf("in loop, something in serial buffer\n");
			chars_read = serial_port_read(read_buffer, MAX_COMMAND_LENGTH); // stuff read buffer into chars_read
		//	printf("in loop, after reading serial buffer\n");
			if(chars_read > 0)
			{
			//	printf("waiting for command\n");
				respond_to_command(read_buffer, chars_read); // do something with the command
			}	
		}	


	}
	return 0;
}

void change_duty_cycle(int level) // TODO TODO TODO TODO NEEDS TO READ ACTUAL PERIOD NOT USE HARD CODED GARBAGE!!!!
{
	int fd;
	char buf[MAX_BUF];
	char str[MAX_STRING_LENGTH];
	int period = 20;
	long duty;

	if(level > 100) {
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO  
	// Print out error message and ask for new number below between 0 and 100
	}

	snprintf(buf, sizeof(buf), PWM_DIR "/duty"); // Create string with device file path

	fd = open(buf, O_WRONLY); // Open file and assign to fd
	if(fd < 0) 
	{
		perror("PWM change_duty_cycle");
		
	}
	
	duty  = (long)level * PERIOD_BASIS * (long)period / 100; //Convert level to percent of period
	snprintf(str, sizeof(str), "%ld", duty); // Convert integer to string for writing
	write(fd, str, MAX_STRING_LENGTH); // Write level into file
		
}

void PWM_run(char *light_switch)
{
	if(strstr(light_switch, "NULL") != NULL)
	{
		perror("ERROR: NULL in PWM_run"); // This should not happen
	}

	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), PWM_DIR "/run"); // change directory into a string
	if(len <0)
	{
		perror("ERROR: BUFFER WRITE in PWM_run");
	}

	fd = open(buf, O_WRONLY); // open up perepherial file
	if(fd < 0)
	{
		perror("PWM_run");
	}

	if(strstr(light_switch, "on") != NULL) // turn PWM on or off
	{
		write(fd, "1", 2);
	}
	else if(strstr(light_switch, "off") != NULL)
	{
		write(fd, "0", 2);
	}

}

void change_period(long period) // period in milliseconds
{
	char str[MAX_STRING_LENGTH];
	char buf[MAX_BUF];
	int fd, len;

	if(period > MAX_PERIOD)
	{
		//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
		// To protect against huge periods
	}

	period = period * PERIOD_BASIS; // converts to milliseconds
	len = snprintf(buf, sizeof(buf), PWM_DIR "/period"); // change directory to string
	if(len < 0)
	{
		perror("ERROR: BUFFER WRITE in change_period");
	}

	fd = open(buf, O_WRONLY); // open up perepherial file
	if(fd < 0)
	{
		perror("ERROR: OPEN FILE in change_period");
	}



	snprintf(str, sizeof(str), "%ld", period); // Convert long to string for writing
	write(fd, str, MAX_STRING_LENGTH); // Write level into file
		
}

void respond_to_command(char *read_buffer, int chars_read)
{	
	int duty_cycle;
	long period;
	
	if(strstr(read_buffer, "period") != NULL)
	{
		printf("change period");	
		serial_port_write("Enter new period\r\n");
		for(;;) // TODO this should probably be wrapped in a function
		{
			chars_read = serial_port_read(read_buffer, MAX_COMMAND_LENGTH); // check serial buffer
			if(chars_read > 0)
			{
				period = atol(read_buffer); // cast to long... this baby is gonna be huge
				change_period(period);
				respond_to_command(read_buffer, chars_read); //This is actually unneccessary TODO remove when stable
				break;
			}
			else
			{
				perror("ERROR: SERIAL PORT READ in respond_to_command");
				break;
			}
		}
	}
	else if(strstr(read_buffer, "dutycycle") != NULL)
	{
		serial_port_write("Enter new period\r\n");
		for(;;) // TODO this should probably be wrapped in a function
		{
			chars_read = serial_port_read(read_buffer, MAX_COMMAND_LENGTH); // check serial buffer
			if(chars_read > 0)
			{
				duty_cycle = atoi(read_buffer); 
				change_duty_cycle(duty_cycle);
				respond_to_command(read_buffer,chars_read); // Again unnecssary TODO remove when stable
				break;
			}
			else
			{
				perror("ERROR: SERIAL PORT READ in respond_to_command");
			}
		}
	}
	else if(strstr(read_buffer, "off") != NULL)
	{
		serial_port_write("off"); // just letting you know
		PWM_run(read_buffer);
	}
	else if(strstr(read_buffer, "on") != NULL)
	{
		serial_port_write("on"); // just letting you know
		PWM_run(read_buffer);
	}
	else if(strstr(read_buffer, "closeapp") != NULL)
	{
		serial_port_close();
		exit(EXIT_SUCCESS); // signal exit
	}

}

void serial_port_close(void)
{
	tcsetattr(serial_port, TCSANOW, &options_original); // assign serial attributes to serial_port
	close(serial_port);
}

int serial_port_open(void)
{
	struct termios options; // create another termios struct

	serial_port = open(PORT_NAME, O_RDWR | O_NONBLOCK); // assign USB0 file handle to serial_port

	if(serial_port != -1) // configure serial
	{
		printf("Serial port open\n");
		tcgetattr(serial_port, &options_original);	
		tcgetattr(serial_port, &options);
		cfsetispeed(&options, B38400);
		cfsetospeed(&options, B38400);
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_lflag |= ICANON;
		tcsetattr(serial_port, TCSANOW, &options);
	}
	else
	{
		printf("ERROR: UNABLE TO OPEN /dev/ttyUSB0\n");
	}
	return(serial_port);
}

int serial_port_read(char *read_buffer, size_t max_chars_to_read) // makes it fancy
{
	int chars_read = read(serial_port, read_buffer, max_chars_to_read); // assign file handle for USB0 to chars_read
	return chars_read;
}

void serial_port_write(char *write_buffer)
{
	int bytes_written;
	size_t len = 0;

	len = strlen(write_buffer); // length in bytes of write_buffer, might put this inside of write
	bytes_written = write(serial_port, write_buffer, len);
	if(bytes_written < len)
	{
		printf("ERROR: SERIAL PORT WRITE \n");
	}
}

void sigint_handler(int sig)
{
	serial_port_close(); 
	exit(sig); // kill everything
}


