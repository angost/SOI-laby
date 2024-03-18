
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	message m;
	int p_pid = 0;
    int new_group = 0;
	if (argc != 3) {
		printf("Wrong number of arguments \n");
    	return 1;
	}
	p_pid = atoi(argv[1]);
	new_group = atoi(argv[2]);

	if (p_pid < 0) {
		printf("Wrong process pid \n");
    	return 1;
	}
	m.m1_i1 = p_pid;

	if (new_group < 0 || new_group > 2) {
		printf("Wrong group nr \n");
		return 1;
	}
	m.m1_i2 = new_group;

	_syscall(MM, SETGROUP, &m);
	printf("Process pid %d new group %d \n", p_pid, new_group);
	return 0;
}

