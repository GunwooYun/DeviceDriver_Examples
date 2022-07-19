/*
    Device Driver LED & KEY on UDOO
    Write function to control LED and get KEY data of UDOO
    file : ledkey_app.c
    device file : /dev/ledkey
    Page: 236
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME  "/dev/ledkey"

int main()
{
    int dev;
    char buff = 15;
    int ret;
    int key_old = 0;

	/* file open */
    dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY );
	/* open error */
	if(dev<0)
	{
		perror("open()");
		return 1;
	}
    ret = write(dev,&buff,sizeof(buff)); // LED ON all
	if(ret < 0)
		perror("write()");
	buff = 0;
	do {
    	ret = read(dev,&buff,sizeof(buff)); // Key read   
  		if(ret < 0)
  			perror("read()");
		/* Runs when different key pressed */
		if(buff != key_old)
		{
			if(buff)
			{
				printf("key_no : %d\n",buff);
				write(dev,&buff,sizeof(buff));
			}
			/* Exit when key 8 pressed */
			if(buff == 8)
				break;
			key_old = buff;
		}
	} while(1);
    close(dev);
    return 0;
}
