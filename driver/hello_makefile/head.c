#include<linux/kernel.h>
#include"head.h"

void test(void){
	printk("hello\n");
}

struct test g_test ={
	.fun = test,
};


