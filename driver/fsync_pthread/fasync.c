#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
//#include <linux/fcntl.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/device.h>

#include"head.h"

#define FASYNC_MINOR 1
#define FASYNC_MAJOR 244
#define DEVICE_NUMBER 1
static struct class * hello_class;
static struct device * hello_class_dev;


//存放任务的容量
#define SIZE 10

struct hello_device
{
	struct fasync_struct *my_fasync;		
	struct cdev cdev;
	
	int deal;
	int tail;
	int head;

	struct task task[SIZE+1];
}my_device;


static int hello_open(struct inode *inode, struct file *file)
{
	printk("driver: hello_open\n");
	return 0;
}

int hello_close(struct inode *inode, struct file *file)
{
	fasync_helper (-1, file, 0, &(my_device.my_fasync));
	return 0;
}

static int hello_fasync (int fd, struct file *filp, int on)
{
	printk("%s %d\n",__FUNCTION__,__LINE__);
	return fasync_helper (fd, filp, on, &(my_device.my_fasync));
}


int add_task(unsigned long buf){
	int ret;
	int tail;
	
	if((my_device.tail+1) % SIZE ==my_device.head){
		printk("task circulatory buffer is full \n");
		return -1;
	}
	
	tail = my_device.tail;
	my_device.tail=(my_device.tail+1) % SIZE;//更新下次要添加的位置
	
	ret=copy_from_user(&my_device.task[tail], (void *)buf, sizeof(struct task));
		
	//处理任务
	{
		my_device.task[tail].data=tail;
	}	
	my_device.deal=my_device.tail;//任务完成,更新下次要处理的索引。
	
	//通知应用层，任务完成。
	kill_fasync (&my_device.my_fasync, SIGIO, POLL_IN);

	return ret;
}


int get_task(unsigned long buf)
{
	int head,ret;
	struct task task_pid;
		
	//没有可以获取的任务
	if(my_device.head==my_device.deal){
		printk("task circulatory buffer is empty \n");
		return -1;
	}
	
	copy_from_user(&task_pid, (void *)buf, sizeof(struct task));
	
	head = my_device.head;
	if(my_device.task[head].pid != task_pid.pid){
		//printk("%s %d my_device.task[head].pid=%d task_pid.pid=%d\n",__FUNCTION__,__LINE__,my_device.task[head].pid,task_pid.pid);
		return -1;
	}
	
	my_device.head =(my_device.head+1) % SIZE;
	printk("get task is :%d %ld %d\n",my_device.task[head].pid,my_device.task[head].tid,my_device.task[head].data);
	ret=copy_to_user((void *)buf, &my_device.task[head],sizeof(struct task));
	
	return 0;
}

long hello_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	if(_IOC_TYPE (cmd) != 'F')
	{
		printk("param error !function:%s line:%d \n ",__FUNCTION__,__LINE__);
		return -1;
	}
	
	switch(_IOC_DIR (cmd)){
		//读取完成的任务
		case _IOC_READ :
						return get_task(arg);
		//写入任务	
		case _IOC_WRITE : 
						return add_task(arg);
		default: 
						printk("param error !function:%s line:%d \n ",__FUNCTION__,__LINE__);
						return -1;
	}
	
}


static struct file_operations hello_fops = {
	.owner   		=  THIS_MODULE,    
	.open   		=  hello_open,     
	.release 		=  hello_close,
	//.read	 		=  hello_read,		
	.unlocked_ioctl =  hello_unlocked_ioctl,
	//.write   		=  hello_write,
	.fasync	 		=  hello_fasync,
};

void init_mydevice(void){
	my_device.head =0;
	my_device.tail = 0;
	my_device.deal = 0;
}
	
static int hello_drv_init(void)
{
	int ret;
	dev_t devno = MKDEV(FASYNC_MAJOR,FASYNC_MINOR);
	
	init_mydevice();
	
	ret = register_chrdev_region(devno, DEVICE_NUMBER, "hello");
	if (ret < 0) {
		printk("failed: register_chrdev_region\n");
		return ret;
	}

	//去掉括号
	cdev_init(&(my_device.cdev), &hello_fops);
	ret = cdev_add(&my_device.cdev, devno, DEVICE_NUMBER);
	if (ret < 0) {
		printk("faield: cdev_add\n");
		return ret;
	}
	
	hello_class = class_create(THIS_MODULE, "hello_class");
	hello_class_dev = device_create(hello_class, NULL, devno, NULL, "hello_device"); 

	return 0;
}

static void hello_drv_exit(void)
{
		
		dev_t devno = MKDEV(FASYNC_MAJOR,FASYNC_MINOR);
		
		device_destroy(hello_class,devno);
		class_destroy(hello_class);
		
		cdev_del(&(my_device.cdev));
		unregister_chrdev_region(devno,DEVICE_NUMBER);
}


module_init(hello_drv_init);
module_exit(hello_drv_exit);

MODULE_LICENSE("GPL");

