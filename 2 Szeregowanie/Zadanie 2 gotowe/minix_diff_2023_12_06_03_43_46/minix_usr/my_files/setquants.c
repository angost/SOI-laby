
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	message m;
	int group = 0;
    int new_quants = 1;
	if (argc != 3) {
		printf("Wrong number of arguments \n");
    	return 1;
	}
	group = m.m1_i1 = atoi(argv[1]);
	new_quants = m.m1_i2 = atoi(argv[2]);
	if (group < 0 || group > 2) {
		printf("Wrong group nr\n");
		return 1;
	} else if (new_quants < 1) {
		printf("Quants should be > 0\n");
		return 1;
	}
	_syscall(MM, SETQUANTS, &m);
	printf("Group %d set quants to %d \n", group, new_quants);
	return 0;
}

