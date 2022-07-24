#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>

#define DEVICE_LED_FILENAME "/proc/ledkey/led"
#define DEVICE_KEY_FILENAME "/proc/ledkey/key"

int main(int argc, char *argv[])
{
	int ledFD;
	int keyFD;
	char buff[10] = {0};

	if(argc < 2)
	{
		printf("Usage : %s [0~127]\n",argv[0]);
		return 1;
	}

	ledFD = open(DEVICE_LED_FILENAME, O_WRONLY );
	if(ledFD < 0)
	{
		perror("open");
		return 2;
	}

	keyFD = open(DEVICE_KEY_FILENAME, O_RDONLY );
	if(keyFD < 0)
	{
		perror("open");
		return 2;
	}
	strcpy(buff,argv[1]);
	write(ledFD,buff,strlen(buff));
	printf("write : %s\n",buff);
	read(keyFD,buff,sizeof(buff));
	printf("read : %s\n",buff);
	close(ledFD);
	close(keyFD);
	return 0;
}
