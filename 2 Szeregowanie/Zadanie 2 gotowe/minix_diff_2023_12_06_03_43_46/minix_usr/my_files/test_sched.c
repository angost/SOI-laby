
#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  message m;
  int i;
  int create_children;
  int groups[20];
  if (argc == 1) {
		printf("Wrong number of arguments \n");
    return 1;
	}
  create_children = atoi(argv[1]);
  

  if (argc != create_children + 2 || argc > 22) {
		printf("Nr of processes doesn't match nr of groups \n");
    return 1;
	}

  for (i = 0; i < create_children; i++){
    groups[i] = atoi(argv[i+2]);
  }

  printf( "creating %d children\n", create_children);
  for (i = 0; i < create_children; ++i) {
    if( fork() == 0 ) {
      m.m1_i1 = getpid();
      m.m1_i2 = groups[i];
      _syscall(MM, SETGROUP, &m);

      m.m1_i1 = getpid();
      _syscall(MM, GETGROUP, &m);

      printf("Process pid: %d Group: %d\n", getpid(), m.m1_i1);
      while(1){
        ;
      }
      sleep(2);
      return 0;
    }
  }
  sleep(2);
  return 0;
}

