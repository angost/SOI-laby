
#include <unistd.h> 
#include <stdio.h>
#include <lib.h>
#include <stdlib.h>

int main() {

	if (fork() == 0) {
		printf("Created child");
		sleep(2);
		return 0;
	}  
	sleep(1);
	return 0;	

}

