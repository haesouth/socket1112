#include <stdio.h>

int main(){
	int a=10;
	int b=100;

	a = a+10;
	sleep(10);

	int pid = fork();

	if(pid>0){//super
		printf("super\n");
		a = a + 10;
		printf("[parent] a = %d, b = %d\n", a, b);
		sleep(20);
	}else if(pid == 0){ //sub
		printf("sub\n");
		b = b * 10;
		printf("[child] a= %d, b=%d\n", a, b);
		sleep(20);
	}else{
		printf("fork()function fail\n");
	}
	return 0;

}
