#ifndef __HEAD_H_
#define __HEAD_H_

struct task{
	int pid;
	unsigned long int  tid;
	int data;
};
//		_IO    an ioctl with no parameters 
//		_IOW   an ioctl with write parameters (copy_from_user)
//		_IOR   an ioctl with read parameters  (copy_to_user)
//		_IOWR  an ioctl with both write and read parameters.

//_IOW  (魔数， 基数， 变量型)
#define IO_WRITE_TASK _IOW('F',0,struct task)
#define IO_READ_TASK _IOR('F',1,struct task)


#endif
