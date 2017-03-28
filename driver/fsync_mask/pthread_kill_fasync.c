
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
pthread_t tid1,tid2;
struct task task1,task2;


pthread_mutex_t mutex;

struct task tt;
void my_signal_fun(int signum)
{
	struct task t;
	int ret;
	
	t.pid=getpid();
	
	ret=ioctl(fd,IO_READ_TASK,&t);
	if(ret!=0)
		return ;
		
	//收到数据了
	pthread_mutex_lock(&mutex);
	tt=t;
	pthread_mutex_unlock(&mutex);

	pthread_kill(t.tid, SIGALRM);
	
}

void pthread_signal_fun(int signum)
{	
	pthread_mutex_lock(&mutex);
	printf("line:%d  current pid=%d  tid=%ld \t get_task: pid=%d  tid=%ld  data=%d\n",__LINE__,getpid(),pthread_self(),tt.pid,tt.tid,tt.data);
	pthread_mutex_unlock(&mutex);
}

void delay_1s(){
	int i ,j;
	for(i=0;i<10000;i++)
		for(j=0;j<10000*2;j++);
}

void *	thread_func1(void * data)
{
	int ret;	
	struct task task0;

	signal(SIGALRM, pthread_signal_fun);

	printf("thread func1 tid1=%ld current=%ld \n",tid1,pthread_self());
	
	
	task0.pid=getpid();
	task0.tid=pthread_self();
	task0.data=0;
	//一秒钟发送一个
	while(1){
		ret=ioctl(fd,IO_WRITE_TASK,&task0);
		if(ret<0){
			printf("send task error! function : %s line : %d\n",__FUNCTION__,__LINE__);
		}
		//printf("line:%d send task is :pid=%d tid=%ld\n",__LINE__,task0.pid,task0.tid);
		
		delay_1s();
	}
}

void *	thread_func2(void * data)
{
	int ret;	
	struct task task0;

	signal(SIGALRM, pthread_signal_fun);

	printf("thread func2 tid2=%ld current=%ld \n",tid2,pthread_self());
	
	
	task0.pid=getpid();
	task0.tid=pthread_self();
	task0.data=0;
	//一秒钟发送一个
	while(1){
		ret=ioctl(fd,IO_WRITE_TASK,&task0);
		if(ret<0){
			printf("send task error! function : %s line : %d\n",__FUNCTION__,__LINE__);
		}
		//printf("line:%d send task is :pid=%d tid=%ld\n",__LINE__,task0.pid,task0.tid);
		
		delay_1s();
	}
}



int main(int argc, char **argv)
{
	int ret;

	int Oflags;
	

	pthread_mutex_init(&mutex,NULL);

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
		
	ret=pthread_create(&tid1, NULL, thread_func1, "thread1");
	if(ret<0){
		perror("pthread_create error\n");
		return -1;
	}
	
	pthread_create(&tid2, NULL, thread_func2, "thread2");
	if(ret<0){
		perror("pthread_create error\n");
		return -1;	
	}

	while(1);
	return 0;
}

