#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/device.h>
#include "head.h"

#define FASYNC_MINOR 1
#define FASYNC_MAJOR 244
#define DEVICE_NUMBER 1
static struct class * hello_class;
static struct device * hello_class_dev;


struct hello_device
{
	struct cdev cdev;
}my_device;

extern struct test g_test;

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
				return -1;
}

static struct file_operations hello_fops = {
	.owner   		=  THIS_MODULE,    
	.open   		=  hello_open,     
	.release 		=  hello_close,
	.unlocked_ioctl =  hello_unlocked_ioctl,
};

static int hello_drv_init(void)
{
	int ret;
	printk("init \n");
	dev_t devno = MKDEV(FASYNC_MAJOR,FASYNC_MINOR);
	
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

	printk("init\n");
	if(g_test.fun==NULL){
		printk("fun is NULL\n");
	}else{
		printk("fun is not NULL\n");
		g_test.fun();
	}
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

