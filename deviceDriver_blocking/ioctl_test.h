/* Avoid duplicated header file */
#ifndef __IOCTL_H__
#define __IOCTL_H__

#define IOCTLTEST_MAGIC 't' // ascii 't' : 116 0x74
typedef struct
{
	unsigned long timer_val;
} __attribute__((packed)) keyled_data; // Remove padding space

#define TIMER_START _IO(IOCTLTEST_MAGIC, 0) 
#define TIMER_STOP _IO(IOCTLTEST_MAGIC, 1) 
#define TIMER_VALUE _IOW(IOCTLTEST_MAGIC, 2, keyled_data) 
#define IOCTLTEST_MAXNR		3 // number of command
#endif
