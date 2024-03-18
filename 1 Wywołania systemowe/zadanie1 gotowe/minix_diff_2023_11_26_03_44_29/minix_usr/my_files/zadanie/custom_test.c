
#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>

int main( int argc, char ** argv)
{
  message m;
  int max;
  pid_t who;
  int pid_to_ignore;
  int create_children;
  int i;
  
  if (argc != 3)
    return 0;
  pid_to_ignore = atoi(argv[1]);
  create_children = atoi(argv[2]);

  printf( "creating %d children\n", create_children);
  for (i = 0; i < create_children; ++i) {
    if( fork() == 0 ) {
      sleep(5);
      return 0;
    }
  }
  sleep(2);


  m.m1_i1 = pid_to_ignore;
  max = _syscall( MM, MAXOFFSPRINGS, &m);
  printf("Max offsprings: %d\n", max);
  m.m1_i1 = pid_to_ignore;
  who = _syscall( MM, WHOMAXOFFSPRINGS, & m );
  printf( "Who: %d\n", who );
  return 0;
}

