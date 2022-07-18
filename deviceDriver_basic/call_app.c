/*
    Device Driver basic
    file : call_app.c
    device file : /dev/calldev
    Page: 170
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

/* Device file */
#define DEVICE_FILENAME "/dev/calldev"

int main(void)
{
	int dev;
	char buff[128];
	int ret;
	printf("1) device file open \n");

	/* Open device file (low level) */
	dev = open(DEVICE_FILENAME, O_RDWR | O_NDELAY); // O_NDELAY : nothing to read, be ready (non-block)

	if(dev >= 0)
	{
		printf("2) seek function call dev : %d \n", dev);
		ret = lseek(dev, 0x20, SEEK_SET); // off_t lseek(int fd, off_t offset, int whence); SEEK_SET <- first
		printf("ret = %#08x \n", ret);
		printf("3) read function call \n");
		ret = read(dev, (char *)0x30, 0x31); // ssize_t read (int fd, void *buf, size_t nbytes);
		printf("ret = %#08X \n", ret);
		printf("4) write function call \n");
		ret = write(dev, (char *)0x40, 0x41); // write(int fd, const void *buf, size_t count)
		printf("ret = %#08X \n", ret);
		printf("5) ioctl function call \n");
		ret = ioctl(dev, 0x51, 0x52); // ioctl(int fd, int request, ...)
		printf("ret = %#08X \n", ret);
		printf("6) device file close \n");
		ret = close(dev); // close(int fd)
		printf("ret = %#08X \n", ret);
	}
	else{
		printf("error code : %d\n", dev);
		perror("open");
	}
	return 0;
}

