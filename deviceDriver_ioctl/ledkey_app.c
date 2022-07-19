/*
    Device Driver using function ioctl
    Use ioctl to control LED and get KEY data of UDOO
    file : ledkey_app.c
    device file : /dev/ledkey
    Page: 294
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
//#include <asm/uaccess.h>
#include "ioctl_test.h"

#define DEVICE_FILENAME  "/dev/ledkey"

int main()
{
    int dev;
    char buff = 15;
    int ret;
    int key_old = 0;

	ioctl_test_info data = {0, {0}};

	//data.size = 1;
	//data.buff[0] = 0x05;


    dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY );
	if(dev<0)
	{
		perror("open()");
		return 1;
	}
	/* #define IOCTLTEST_MAGIC 't' <-- 0x74 */
	/* #define IOCTLTEST_KEYLEDINIT _IO(IOCTLTEST_MAGIC, 0) */
	/* # define _IO(x,y)	(((x)<<8)|y) */
	ret = ioctl(dev,IOCTLTEST_KEYLEDINIT); // 0x7400

	/* #define IOCTLTEST_LEDON     _IO(IOCTLTEST_MAGIC, 3) */
	ret = ioctl(dev,IOCTLTEST_LEDON); // 0x7403
	sleep(1);
	/* #define IOCTLTEST_LEDOFF    _IO(IOCTLTEST_MAGIC, 2) */
	ret = ioctl(dev,IOCTLTEST_LEDOFF); // 0x7402
	sleep(1);
	ret = ioctl(dev,IOCTLTEST_LEDON);
	sleep(1);
	ret = ioctl(dev,IOCTLTEST_LEDOFF);
	sleep(1);

	data.size = 1;
	data.buff[0] = 0x05;
	ret = ioctl(dev,IOCTLTEST_LEDWRITE, &data);
	sleep(2);
	data.buff[0] = 0x00;
	ret = ioctl(dev,IOCTLTEST_LEDWRITE, &data);

	/* #define IOCTLTEST_KEYLEDFREE _IO(IOCTLTEST_MAGIC, 1) */
	ret = ioctl(dev,IOCTLTEST_KEYLEDFREE); // 0x7401

/*

    ret = write(dev,&buff,sizeof(buff));
	if(ret < 0)
		perror("write()");
	buff = 0;
	do {
    	ret = read(dev,&buff,sizeof(buff));              
  		if(ret < 0)
  			perror("read()");
		if(buff != key_old)
		{
			if(buff)
			{
				printf("key_no : %d\n",buff);
				write(dev,&buff,sizeof(buff));
			}
			if(buff == 8)
				break;
			key_old = buff;
		}
	} while(1);
*/
    close(dev);
    return 0;
}
