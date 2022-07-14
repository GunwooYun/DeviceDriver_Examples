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
//#define IOCTLTEST_KEYLEDFREE _IO(IOCTLTEST_MAGIC, 1)
//#define IOCTLTEST_LEDOFF	_IO(IOCTLTEST_MAGIC, 2)
//#define IOCTLTEST_LEDON		_IO(IOCTLTEST_MAGIC, 3)
/* 0100 0000 1000 0100 0111 0100 0000 0100*/
/*  4     0   8    4    7    4    0    4  */
//#define IOCTLTEST_LEDWRITE	_IOW(IOCTLTEST_MAGIC, 4, ioctl_test_info)
//#define IOCTLTEST_INTVAL	_IOR(IOCTLTEST_MAGIC, 5, ioctl_test_info)
#define IOCTLTEST_MAXNR		3 // number of command
#endif
