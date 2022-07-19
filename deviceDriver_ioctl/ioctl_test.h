/* Avoid duplicated header file */
#ifndef __IOCTL_H__
#define __IOCTL_H__

#define IOCTLTEST_MAGIC 't' // ascii 't' : 116 0x74
typedef struct
{
	unsigned long size;
	unsigned char buff[128];
} __attribute__((packed)) ioctl_test_info; // Remove padding space

#define IOCTLTEST_KEYLEDINIT _IO(IOCTLTEST_MAGIC, 0) // _IO : command without data
#define IOCTLTEST_KEYLEDFREE _IO(IOCTLTEST_MAGIC, 1)
#define IOCTLTEST_LEDOFF	_IO(IOCTLTEST_MAGIC, 2)
#define IOCTLTEST_LEDON		_IO(IOCTLTEST_MAGIC, 3)
/* cmd */
/* 0100 0000 1000 0100 0111 0100 0000 0100*/
/*  4     0   8    4    7    4    0    4  */
#define IOCTLTEST_LEDWRITE	_IOW(IOCTLTEST_MAGIC, 4, ioctl_test_info) // _IOW : command write with data
#define IOCTLTEST_MAXNR		5 // number of command
#endif
