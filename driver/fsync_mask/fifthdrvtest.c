
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


/* fifthdrvtest 
  */
int fd;

void my_signal_fun(int signum)
{
	unsigned char key_val;
	read(fd, &key_val, 1);
	printf("key_val: 0x%x\n", key_val);
}

int main(int argc, char **argv)
{
	unsigned char key_val;
	int ret;
	int Oflags;

	//第一步
	//注册信号处理函数，驱动会主动的往该进程发信号
	signal(SIGIO, my_signal_fun);
	
	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	//第二部，下面三句代码
	//设置设备文件的拥有者是本进程，即告诉驱动程序信号往这个进程发
	fcntl(fd, F_SETOWN, getpid()); 
	Oflags = fcntl(fd, F_GETFL); 
	//设置设备文件支持fasync，该操作将导致驱动的fasync函数被调用
	fcntl(fd, F_SETFL, Oflags | FASYNC);


	while (1)
	{
		sleep(1000);
	}
	
	return 0;
}

