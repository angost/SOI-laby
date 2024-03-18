
#include <stdio.h>
#include <stdlib.h>
#include <lib.h>

int main( int argc, char ** argv)
{
  int pid_to_ignore;
  message m;
  int max;
  pid_t who;
  
  if (argc != 2)
    return 0;
  pid_to_ignore = atoi(argv[1]);

  m.m1_i1 = pid_to_ignore;
  max = _syscall( MM, MAXOFFSPRINGS, & m );
  printf( "Max nr of offsprings: %d\n", max );
  m.m1_i1 = pid_to_ignore;
  who = _syscall( MM, WHOMAXOFFSPRINGS, & m );
  printf( "Who: %d\n", who );
  return 0;
}

