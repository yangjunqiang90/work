#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/list.h>

#define FASYNC_MINOR 1
#define FASYNC_MAJOR 244
#define DEVICE_NUMBER 1

static struct class * hello_class;
static struct device * hello_class_dev;

typedef struct Student {
	int sid;
	struct list_head list;
}Student; 

struct list_head students;

struct hello_device
{

	struct cdev cdev;
}my_device;


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
	printk("driver: hello_ioctl\n");
	return 0;	
}

static struct file_operations hello_fops = {
	.owner   		=  THIS_MODULE,    
	.open   		=  hello_open,     
	.release 		=  hello_close,
	.unlocked_ioctl =  hello_unlocked_ioctl,
};


void insert(struct list_head * new,struct list_head * head){
	Student *p;
	Student *q;

	//new Student
	Student *n;
	//链表为空，直接插入。
	if(head->next == head){
		list_add(new,head);
		return ;
	}

	n = container_of(new,Student,list);
/*
 	//和下面的从重复了，这段代码不用了。
	//链表只有一个元素
	if(head->next->next == head){
		tmp = container_of(head->next,Student,list);
		if(tmp->sid < n->sid){
		//插到tmp的后面
		__list_add(new,head->next,head);
		}else{
		//插到tmp的前面
		__list_add(new,head,head->next);
		}
		return ;		
	}
*/
	//链表有多个元素
	//先处理是否插入到第一个节点的前面（也就是头结点的后面）或者最后一个节点的后面
	
	p = container_of(head->prev,Student,list);
	if(p->sid < n->sid){
		__list_add(new,head->prev,head);	
		return ;
		
	}

	p = container_of(head->next,Student,list);
	if(p->sid > n->sid){
		//插入到头结点的后面
		__list_add(new,head,head->next);	
		return ;
	}
	

	while(1){
		q=container_of(p->list.next,Student,list);
		if(n->sid >= p->sid && n->sid <= q->sid){
			__list_add(new,&(p->list),&(q->list));
			break;
		}
		p=q;
	}
}


static int hello_drv_init(void)
{
	int ret;
	int i;
	Student *s = NULL;
	Student *t;
	struct list_head *pos;
	struct list_head *n;

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
	
	hello_class = class_create(THIS_MODULE, "hello_class");
	hello_class_dev = device_create(hello_class, NULL, devno, NULL, "hello_device"); 


	INIT_LIST_HEAD(&students);

	for(i=30;i>0;i--){
		s = kmalloc(sizeof(Student),GFP_KERNEL);
		if(s == NULL){
			printk("kmalloc %d error \n",i);
			return -1;
		}

		s->sid = i;
		if(i==19){
			s->sid=11;
		}
		//从链表尾部插入，添加到students->prev之后，students之前
		//list_add_tail(&(s->list),&students);
		//从链表头部插入，添加到students之后，student->next之前
		//list_add(&(s->list),&students);
		insert(&(s->list),&students);
	}
	
	//第一个参数作为了等号的左直，然后又取地址，因此这样的写法时错误的。
	//list_for_each((&(tmp.list)),&(students)){
	
	//遍历链表，pos是遍历的每一个结果
	list_for_each(pos,&(students)){
		t = container_of(pos,Student ,list);
		printk("student id is:%d \n",t->sid);
	}
	
	//这里要释放链表中的元素，使用list_for_each就会出错。
	list_for_each_safe(pos,n,&students){
		list_del(pos);
		s=list_entry(pos,Student,list);
		kfree(s);
	}
	//这里是局部变量，就不用释放了，正常应该把链表头也释放掉
	//kfree(&students);

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

