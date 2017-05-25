#include<signal.h>
#include<stdio.h>

#define DEBUG 

#ifdef DEBUG
	#define DEBUG_PRINT(format,args...) \
	do{  \
		printf("[%s:%d] ",__FUNCTION__,__LINE__); \
		printf(format,##args); \
	}while(0)
#else
	#define DEBUG_PRINT(format,args...) 
#endif


int main(){
	unsigned char i=250;
	unsigned char k = 1;
	unsigned char result;
	int j ;
	for(j=0;j<10;j++)
	DEBUG_PRINT("%d ",i++);

	printf("\n");
	i=250;
	result = k-i;
	DEBUG_PRINT("%d - %d = %d\n",k,i,result);
	return 0;
}


