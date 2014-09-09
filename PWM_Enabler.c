/*		PWM ENABLER
 *		Roman Nelson
 *		Voss Lighting 2014
 *
 *	Enables PWM on ARM335
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include "PWM_Enabler.h"


void enable_PWM(void) 
{
/*enable_PWM
 * turns on clock for EHRPWM 0A, @P9_21
 */
	volatile uint32_t *pwm_access; // Holds address for memory map
	int PWM_file, fd; // Memory map file handle, device parameter file, and length variable
	char buf[MAX_BUF];
	char str[MAX_STRING_LENGTH];

	PWM_file = open("/dev/mem", O_RDWR); // Open file for mmap and assign handle to PWM_file
	if(PWM_file == -1) {
		perror("open failed");
	}

	pwm_access = (volatile uint32_t *)mmap(NULL, CM_PER_REG_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, PWM_file, CM_PER_REG_START); // Create mapping assign to pwm_access
	if(pwm_access == (volatile uint32_t *)MAP_FAILED) {
		perror("mmap failed");
		close(PWM_file);
	}

	pwm_access[PWM_0A] = PWM_CLOCK_ENABLE; // Turn it on!	
	close (PWM_file);

	snprintf(buf, sizeof(buf), PWM_DIR "/polarity"); // Create string with device file path
	fd = open(buf, O_WRONLY); // Open file and assign to fd	
	if(fd < 0)
	{
		perror("enable_PWM Bro");
	}

	snprintf(str, sizeof(str), "%d", 0);	
	write(fd, str, 3); //
        close(fd);	

}	
