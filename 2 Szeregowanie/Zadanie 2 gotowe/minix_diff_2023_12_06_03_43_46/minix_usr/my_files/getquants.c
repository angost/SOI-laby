
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	message m;
	int group = 0;
	if (argc != 2) {
		printf("Wrong number of arguments \n");
    	return 1;
	}
	group = m.m1_i1 = atoi(argv[1]);
	if (group < 0 || group > 2) {
		printf("Wrong group nr\n");
		return 1;
	}
    /*check if procnr <0, nrproc>, else einval*/
	_syscall(MM, GETQUANTS, &m);
	printf("Group %d quants %d \n", group, m.m1_i1);
	return 0;

}

