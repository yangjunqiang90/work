#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/timer.h>

#include"head.h"

#define FASYNC_MINOR 1
#define FASYNC_MAJOR 244
#define DEVICE_NUMBER 1

static struct class * hello_class;
static struct device * hello_class_dev;


struct hello_device
{
	struct timer_list timer;
	spinlock_t spin_lock ;
	wait_queue_head_t  hello_wq;
	struct cdev cdev;
}my_device;

static int wake_pid;

void timer_function(unsigned long data){
	spin_lock(&my_device.spin_lock);
	wake_pid=current->pid;
	spin_unlock(&my_device.spin_lock);
				
	wake_up_interruptible(&my_device.hello_wq);
}

static int hello_open(struct inode *inode, struct file *file)
{
	printk("driver: hello_open\n");
	return 0;
}

int hello_close(struct inode *inode, struct file *file)
{
	printk("driver: hello_close\n");
	return 0;
}


long hello_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	static int count;
	if(_IOC_TYPE (cmd) != 'F')
	{
		printk("param error !function:%s line:%d \n ",__FUNCTION__,__LINE__);
		return -1;
	}
	
	switch(_IOC_DIR (cmd)){
		case _IOC_READ :
				return 0;

		case _IOC_WRITE : 
				if((count++)%2==1){
					//(1)还没有调用check_finish时，任务就完成了。
					spin_lock(&my_device.spin_lock);
					wake_pid=current->pid;
					spin_unlock(&my_device.spin_lock);
				
					wake_up_interruptible(&my_device.hello_wq);
				}else{
					//(2)调用check_finish时任务还没有完成。启动一个定时器，模拟。
					mod_timer(&my_device.timer,jiffies+HZ*3);	
				}
				return 0;
		default: 
				printk("param error !function:%s line:%d \n ",__FUNCTION__,__LINE__);
				return -1;
	}
	
}
static unsigned int hello_poll(struct file *file, struct poll_table_struct *table)
{
	unsigned  int  mask=0;

	//poll_wait(file,&my_device.hello_wq,table);//必须在这个位置
	wait_event_interruptible(&my_device.hello_wq,wake_pid==current->pid);

	//disable_irq();
	spin_lock_irq(&my_device.spin_lock);
	if(wake_pid==current->pid)
	wake_pid=0;
	spin_unlock(&my_device.spin_lock);
				

	mask  |= POLLIN;
	
	return  mask;
}
static struct file_operations hello_fops = {
	.owner   		=  THIS_MODULE,    
	.open   		=  hello_open,     
	.release 		=  hello_close,
	.unlocked_ioctl =  hello_unlocked_ioctl,
	.poll 			=  hello_poll,
};

static int hello_drv_init(void)
{
	int ret;
	dev_t devno = MKDEV(FASYNC_MAJOR,FASYNC_MINOR);
	
	ret = register_chrdev_region(devno, DEVICE_NUMBER, "hello");
	if (ret < 0) {
		printk("failed: register_chrdev_region\n");
		return ret;
	}

	cdev_init(&my_device.cdev, &hello_fops);
	ret = cdev_add(&my_device.cdev, devno, DEVICE_NUMBER);
	if (ret < 0) {
		printk("faield: cdev_add\n");
		return ret;
	}
	
	init_waitqueue_head(&my_device.hello_wq);
	spin_lock_init(&my_device.spin_lock);

	init_timer(&my_device.timer);
	my_device.timer.function=timer_function;

	
	hello_class = class_create(THIS_MODULE, "hello_class");
	hello_class_dev = device_create(hello_class, NULL, devno, NULL, "hello_device"); 

	return 0;
}

static void hello_drv_exit(void)
{
	dev_t devno = MKDEV(FASYNC_MAJOR,FASYNC_MINOR);	
	
	device_destroy(hello_class,devno);
	class_destroy(hello_class);

	del_timer(&my_device.timer);	

	cdev_del(&(my_device.cdev));
	unregister_chrdev_region(devno,DEVICE_NUMBER);
}


module_init(hello_drv_init);
module_exit(hello_drv_exit);

MODULE_LICENSE("GPL");

