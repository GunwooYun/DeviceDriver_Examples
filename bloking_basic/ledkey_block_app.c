#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "ioctl_test.h"

#define DEVICE_FILENAME  "/dev/ledkey_block"

int main()
{
    int dev;
    char buff = 15;
    int ret;
    int key_old = 0;
	ioctl_test_info info={0,{0}};

    //dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY ); // Remove O_NDELAY
    //dev = open( DEVICE_FILENAME, O_RDWR|O_NONBLOCK); // O_NDELAY == O_NONBLOCK
    dev = open( DEVICE_FILENAME, O_RDWR); // Blocking Mode 
	if(dev<0)
	{
		perror("open()");
		return 1;
	}
	/* Light on all */
    ret = write(dev,&buff,sizeof(buff));
	/* Failed to write */
	if(ret < 0){
		perror("write()");
		return -1;
	}
	buff = 0;
	do {
    	ret = read(dev,&buff,sizeof(buff));              
		ioctl(dev, IOCTLTEST_INTVAL, &info);
		printf("intaval : %.2lf(s) \n", info.time/100.0);
  		if(ret < 0){
  			perror("read()");
			return -1;
		}
		/* ret shoud be 1 ~ 8 */
		//if(buff == 0) // sw_no == 0
	//		continue;
	//	if(buff != key_old)
	//	{
		printf("key_no : %d\n",buff);
		write(dev,&buff,sizeof(buff));
		if(buff == 8)
			break;
	//		key_old = buff;
	//	}
	} while(1);
    close(dev);
    return 0;
}
