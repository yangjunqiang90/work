
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include"head.h"

static int fd;


int main(int argc, char **argv)
{
	int ret,i,j;
	int Oflags;
	
	fd = open("/dev/hello_device", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	while(1){
	}

	return 0;
}

