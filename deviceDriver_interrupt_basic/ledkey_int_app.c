/*
   Device Driver Interrupt
   Read GPIO data using interrupt
   file : ledkey_int_app.c
   device file : /dev/ledkey_int
   page : p.343
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME  "/dev/ledkey_int"

int main()
{
    int dev;
    char buff = 15;
    int ret;
    int key_old = 0;

    dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY );
	if(dev<0)
	{
		perror("open()");
		return 1;
	}
    ret = write(dev,&buff,sizeof(buff));
	/* Failed to write */
	if(ret < 0){
		perror("write()");
		return -1;
	}
	buff = 0;
	do {
    	ret = read(dev,&buff,sizeof(buff));              
  		if(ret < 0){
  			perror("read()");
			return -1;
		}
		if(buff == 0) // sw_no == 0
			continue;
		if(buff != key_old)
		{
			printf("key_no : %d\n",buff);
			write(dev,&buff,sizeof(buff));
			if(buff == 8)
				break;
			key_old = buff;
		}
	} while(1);
    close(dev);
    return 0;
}
