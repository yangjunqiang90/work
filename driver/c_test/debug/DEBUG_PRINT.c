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
	int i=12;
	DEBUG_PRINT("%d\n",i);
	return 0;
}
