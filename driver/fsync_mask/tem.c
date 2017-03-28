
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

void my_signal_fun(int signum)
{
	struct task t;
	int ret;
	
	t.pid=getpid();
	t.tid=pthread_self();	
	
	ret=ioctl(fd,IO_READ_TASK,&t);
	if(ret!=0)
		return ;
	
	printf("current pid=%d  tid=%d \t get_task: pid=%d  tid=%d  data=%d ret%d \n",getpid(),pthread_self(),t.pid,t.tid,t.data,ret);
	
		
}

int main(int argc, char **argv)
{
	int ret;
	int Oflags;
	struct task task0;
	
	task0.pid=getpid();
	task0.tid=pthread_self();
	task0.data=0;

	//第一步
	//注册信号处理函数，驱动会主动的往该进程发信号
	signal(SIGIO, my_signal_fun);
	
	fd = open("/dev/hello_device", O_RDWR);
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
		
	//一秒钟发送一个
	
	while(1){
		ret=ioctl(fd,IO_WRITE_TASK,&task0);
		if(ret<0){
			printf("function : %s line : %d ret : %d\n",__FUNCTION__,__LINE__,ret);
		}
		sleep(1);
	}

	return 0;
}

