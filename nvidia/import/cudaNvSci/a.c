#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include "stdarg.h"
#include <stdio.h>
  #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

int main()
{
	open("123",1,23);
	open("123",1);
		ioctl(2,3,3);
		ioctl(123,456,789);
		ioctl(321,234);

	return 0;
}
