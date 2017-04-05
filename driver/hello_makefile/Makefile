$(warning KERNELRELEASE=$(KERNELRELEASE))
ifeq ($(KERNELRELEASE),)

KERNELDIR ?= /lib/modules/$(shell uname -r)/build  
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules*

.PHONY: all clean

else 
 
 #注意：必须没有myhello.c这个文件。生成的模块名字是myhello.ko
 obj-m += myhello.o  
 # 模块依赖的所有文件
 myhello-objs :=head.o hello.o

endif  
