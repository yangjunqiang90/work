
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
#include<sys/time.h>

#include"head.h"

static int fd;
pthread_t tid1,tid2;
static int count = 10;

void delay_1s(){
	int i ,j;
	for(i=0;i<10000;i++)
		for(j=0;j<10000*2;j++);
}

int check_finish(){
	fd_set fset;
	FD_ZERO(&fset);
	FD_SET(fd,&fset);

	return select(fd+1,&fset,NULL,NULL,NULL);
}

int open_device(){
	fd = open("/dev/hello_device", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
		return -1;
	}
	return 0;
}

int write_data(){
	int ret;
	ret=ioctl(fd,IO_WRITE_TASK,NULL);
	if(ret<0){
		return -1;
	}
	return 0;
}

void *	thread_func(void * data)
{
	int ret;	

	ret=open_device();
	if(ret!=0){
		return (void *)-1;
	}

	while(1){
		write_data();
		check_finish();
		
		printf("read data\n");

		delay_1s();//延迟一下,再发送下一个任务

		//发送10个任务
		if(--count==0){
			count=10;
			break;
		}
	}
}

int main(int argc, char **argv)
{
	int ret;
		
	ret=pthread_create(&tid1, NULL, thread_func, "thread1");
	if(ret<0){
		perror("pthread_create error\n");
		return -1;
	}
	
	while(1){
		printf("hello\n");
		sleep(10);
	}
	return 0;
}

