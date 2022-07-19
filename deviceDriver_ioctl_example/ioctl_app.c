/*
    Device Driver ioctl on UDOO
    ioctl cmd controls LED on/off, reads key
    file : ioctl_app.c
    device file : /dev/ioctldev
    Page: 302
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "ioctl_test.h"
#define DEVICE_FILENAME "/dev/ioctldev"
int main()
{
	ioctl_test_info info={0,{0}};
	int dev;
	int state;
	int cnt;
	int ret;

	dev = open( DEVICE_FILENAME, O_RDWR|O_NDELAY );
	if( dev >= 0 )
	{
		/* KEY LED 초기화 */
		ioctl(dev, IOCTLTEST_KEYLEDINIT );
		printf("IOCTLTEST_KEYLEDINIT : %x\n",IOCTLTEST_KEYLEDINIT);
		printf( "wait... input1\n" );
		ioctl(dev, IOCTLTEST_LEDON ); // LED ON
		while(1)
		{
			state = ioctl(dev, IOCTLTEST_GETSTATE ); // return key
			if(state == 1) break;
		}
		/* LED OFF */
		ioctl(dev, IOCTLTEST_LEDOFF );
		sleep(1);
		printf( "wait... input2\n" );
		while(1)
		{
			info.size = 0;
			/* 키 값 kernel->user */
			ioctl(dev, IOCTLTEST_READ, &info );
			if( info.size > 0 )
			{
				printf("key : %d\n",info.buff[0]);

				if(info.buff[0] == 1) break; // 키 1번이 눌러졌을 경우 break
			}
		}
		info.size = 1;
		info.buff[0] = 0x0F;
		for( cnt=0; cnt<10; cnt++ )
		{
			ioctl(dev, IOCTLTEST_WRITE, &info ); // data user->kernel, led on
			info.buff[0] = ~info.buff[0];
			usleep( 500000 );
		}
		printf( "wait... input3\n" );
		cnt = 0;
		state = 0xFF;
		while(1)
		{
			info.size = 1;
			info.buff[0] = state;
			/* write first, then read data */
			ret = ioctl(dev, IOCTLTEST_WRITE_READ, &info );
			/* error 일경우 */
			if(ret < 0)
			{
				printf("ret : %d\n",ret);
				perror("ioctl()");
			}
			/* 받아온 info의 size 가 0보다 클 경우 */
			if( info.size > 0 )
			{
				printf("key : %d\n",info.buff[0]);
				if(info.buff[0] == 1) break; // 키 1번이 눌러졌을 경우 break
			}
			state = ~state; // 반전, LED ON -> LED OFF -> LED ON -> ...
			usleep( 100000 );
		}
		ioctl(dev, IOCTLTEST_LEDOFF );
		ioctl(dev, IOCTLTEST_KEYLEDFREE );
  		close(dev);
	}
	else
	{
		perror("open");
		return 1;
	}
	return 0;
}
