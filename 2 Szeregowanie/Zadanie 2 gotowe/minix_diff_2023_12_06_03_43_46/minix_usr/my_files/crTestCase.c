
#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  message m;
  int i, current_pid;
  int quantsA, quantsB, quantsC, countA, countB, countC;

  /*getting and verifying arguments*/
  if (argc != 7) {
		printf("Wrong number of arguments \n");
    return 1;
	}
  quantsA = atoi(argv[1]); quantsB = atoi(argv[2]); quantsC = atoi(argv[3]);
  if (quantsA <= 0 || quantsB <= 0 || quantsC <= 0){
    printf("Nr of quants has to be positive \n");
    return 1;
  }
  countA = atoi(argv[4]); countB = atoi(argv[5]); countC = atoi(argv[6]);
  if (countA < 0 || countB < 0 || countC < 0){
    printf("Nr of processes in group cannot be negative \n");
    return 1;
  }
  if (countA == 0 && countB == 0 && countC == 0){
      printf("Nr of processes in group cannot be negative \n");
      return 1;
    }


  /*setting quants for groups*/
  m.m1_i1 = 0; /*group nr*/
	m.m1_i2 = quantsA; /*quants*/
  _syscall(MM, SETQUANTS, &m);

  m.m1_i1 = 1; /*group nr*/
	m.m1_i2 = quantsB; /*quants*/
  _syscall(MM, SETQUANTS, &m);

  m.m1_i1 = 2; /*group nr*/
	m.m1_i2 = quantsC; /*quants*/
  _syscall(MM, SETQUANTS, &m);

  /*creating processes*/

  for (i = 0; i < countA; ++i) {
    if( fork() == 0 ) {
      m.m1_i1 = getpid();
      m.m1_i2 = 0;
      _syscall(MM, SETGROUP, &m);
      printf("Process %d group %d \n", getpid(), 0);
      while(1) {;}
      return 0;
    }
  }
  for (i = 0; i < countB; ++i) {
    if( fork() == 0 ) {
      m.m1_i1 = getpid();
      m.m1_i2 = 1;
      _syscall(MM, SETGROUP, &m);
      printf("Process %d group %d \n", getpid(), 1);
      while(1) {;}
      return 0;
    }
  }
  for (i = 0; i < countC; ++i) {
    if( fork() == 0 ) {
      m.m1_i1 = getpid();
      m.m1_i2 = 2;
      _syscall(MM, SETGROUP, &m);
      printf("Process %d group %d \n", getpid(), 2);
      while(1) {;}
      return 0;
    }
  }
  return 0;
}

