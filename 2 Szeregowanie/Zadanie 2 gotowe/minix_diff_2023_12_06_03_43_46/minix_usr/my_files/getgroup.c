
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	message m;
	int p_pid = 0;
	if (argc != 2) {
		printf("Wrong number of arguments \n");
    	return 1;
	}
	p_pid = atoi(argv[1]);
	if (p_pid < 0) {
		printf("Wrong process pid \n");
    	return 1;
	}
	m.m1_i1 = p_pid;

	_syscall(MM, GETGROUP, &m);
	printf("Process pid %d group %d \n", p_pid, m.m1_i1);
	return 0;
}

