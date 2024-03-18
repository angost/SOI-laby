
#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>

int main()
{
  message m;
  int i;
  int create_children = 5;
  int groups[5] = {0,0,0,1,2};

  printf( "creating %d children\n", create_children);
  for (i = 0; i < create_children; ++i) {
    if( fork() == 0 ) {
      m.m1_i1 = getpid();
      m.m1_i2 = groups[i];
      _syscall(MM, SETGROUP, &m);

      m.m1_i1 = getpid();
      _syscall(MM, GETGROUP, &m);

      printf("Child pid: %d Group: %d\n", getpid(), m.m1_i1);
      sleep(2);
      return 0;
    }
  }
  sleep(2);
  return 0;
}

