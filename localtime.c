#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

int main(int argc, char *argv[])
{
		struct tm *t;
		time_t tt; // type long

		tt = time(NULL); // Return sceonds from 1970.01.01 00:00
		if(errno == EFAULT)
				perror("time\n");

		// GMT Time
		t = gmtime(&tt); // Convert seconds to GMT string
		printf("gmt time   : %s", asctime(t)); // asctime <- string : ascii type

		// Local Time
		t = localtime(&tt); // convert GMT to LOCAL
		printf("local time : %s", asctime(t));
		printf("local time : Hour:%d, Min:%d, Sec:%d\n",t->tm_hour,t->tm_min,t->tm_sec);

		return 0;
}
