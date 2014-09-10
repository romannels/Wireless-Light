#ifndef __PWM_ENABLER__
#define __PWM_ENABLER__



/* CONSTANTS */

#define CM_PER_REG_START 0x44e10000 // Starting register address
#define CM_PER_REG_LENGTH 1024 // memory length
#define PWM_0A 0x154 //EHRPWM 0A Offset
#define PWM_CLOCK_ENABLE 0x2 // Enable PWM 
#define PWM_DIR "/sys/devices/ocp.2/pwm_test_P8_13.14" // Device directory
#define MAX_BUF 64 // for serial buffer
#define MAX_STRING_LENGTH 15 // For conversion between integer and string
#define PERIOD_BASIS 1000000 // Corresponds to 1 ms period
#define MAX_PERIOD 100 // Arbitrarily selected TODO NEED TO RESEARCH MAX PERIOD
#define MAX_COMMAND_LENGTH 10 // for serial input

/* PROTOTYPES */
void enable_PWM(void); 

#endif
