
#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>

/* usage - ./mTestCase pid1 pid2 pid3 pid4 ....  */
int main(int argc, char* argv[])
{
  message m;
  int i, current_pid, current_group, ticks;
  int ticksA = 0; int ticksB = 0; int ticksC = 0;

  if (argc == 1) {
		printf("Wrong number of arguments \n");
    return 1;
	}

  /*dla kazdego pidu - wypisz ticki -> poczekaj -> wypisz nowe ticki*/
  for (i = 1; i < argc; i++){
    current_pid = atoi(argv[i]);

    m.m1_i1 = current_pid;
	  _syscall(MM, GETTICKS, &m);
    ticks = m.m1_i1;

    m.m1_i1 = current_pid;
	  _syscall(MM, GETGROUP, &m);
    current_group = m.m1_i1;

    if (current_group == 0) {
      ticksA += ticks;
    } else if (current_group == 1) {
      ticksB += ticks;
    } else {
      ticksC += ticks;
    }
    /*printf("pid %d group %d ticks %d | ", current_pid, current_group, ticks);*/
  }
  printf("ticks A %d ticks B %d ticks C %d \n", ticksA, ticksB, ticksC);
  return 0;
}

