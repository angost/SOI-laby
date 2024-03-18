/* Miscellaneous system calls.				Author: Kees J. Bot
 *								31 Mar 2000
 *
 * The entry points into this file are:
 *   do_reboot: kill all processes, then reboot system
 *   do_svrctl: memory manager control
 */

#include "mm.h"
#include <minix/callnr.h>
#include <signal.h>
#include <sys/svrctl.h>
#include "mproc.h"
#include "param.h"

int countOffspringsRecursive(int direct_parent_index, int previous_nr_of_children) {
    int new_nr_of_children = previous_nr_of_children;
    int process_index;
    for ( process_index = 0; process_index < NR_PROCS; process_index++) {
        if ((mproc[process_index].mp_flags & IN_USE) && process_index != direct_parent_index && (mproc[process_index].mp_parent == direct_parent_index) && (mproc[process_index].mp_pid != 0)) {
            new_nr_of_children++;
            new_nr_of_children = countOffspringsRecursive(process_index, new_nr_of_children);
        }
    }

    return new_nr_of_children;
}

void findMaxOffsprings(int * children, pid_t * who, pid_t ignore_pid, pid_t * was_ignored) {

    int max_nr_of_children = -1;
    int nr_of_children = 0;
    int process_index;
    pid_t found;
    pid_t ignored;

    for (  process_index = 0; process_index < NR_PROCS; process_index++) {
        if ((mproc[process_index].mp_flags & IN_USE) && (mproc[process_index].mp_pid != ignore_pid) && (mproc[process_index].mp_pid != 0))
        {
            printf("Scanning pid %d  |", mproc[process_index].mp_pid);

            nr_of_children = 0;
            nr_of_children = countOffspringsRecursive(process_index, nr_of_children);

            if (nr_of_children > max_nr_of_children) {
                max_nr_of_children = nr_of_children;
                found = mproc[process_index].mp_pid;
                printf("CURRMAX children %d proc %d  ", max_nr_of_children, found );
            }
        } else {
            ignored = mproc[process_index].mp_pid;
            printf("Ignored pid %d ", ignored);
            if (mproc[process_index].mp_flags & IN_USE){
              printf("goal |");
            } else {
              printf("inact |");
            }
        }
    }
    
    *children = max_nr_of_children;
    *who = found;
    *was_ignored = ignored;
}

PUBLIC int do_maxOffsprings()
{
  int offsprings = -1;
  pid_t who = -1;
  pid_t ignore_pid = mm_in.m1_i1;
  pid_t was_ignored = -1;
  findMaxOffsprings( & offsprings , & who, ignore_pid, & was_ignored );
  return offsprings;
}

PUBLIC int do_whoMaxOffsprings()
{
  int offsprings = -1;
  pid_t who = -1;
  pid_t ignore_pid = mm_in.m1_i1;
  pid_t was_ignored = -1;
  findMaxOffsprings( & offsprings , & who, ignore_pid, & was_ignored );
  return who;
}

/*=====================================================================* 
 *                       do_reboot				       *
 *=====================================================================*/
PUBLIC int do_reboot()
{
  register struct mproc *rmp = mp;
  char monitor_code[32*sizeof(char *)];

  if (rmp->mp_effuid != SUPER_USER) return(EPERM);

  switch (reboot_flag) {
  case RBT_HALT:
  case RBT_REBOOT:
  case RBT_PANIC:
  case RBT_RESET:
	break;
  case RBT_MONITOR:
	if (reboot_size >= sizeof(monitor_code)) return(EINVAL);
	if (sys_copy(who, D, (phys_bytes) reboot_code,
		MM_PROC_NR, D, (phys_bytes) monitor_code,
		(phys_bytes) (reboot_size+1)) != OK) return(EFAULT);
	if (monitor_code[reboot_size] != 0) return(EINVAL);
	break;
  default:
	return(EINVAL);
  }

  /* Kill all processes except init. */
  check_sig(-1, SIGKILL);

  tell_fs(EXIT, INIT_PROC_NR, 0, 0);	/* cleanup init */
  tell_fs(EXIT, MM_PROC_NR, 0, 0);	/* cleanup for myself */

  tell_fs(SYNC,0,0,0);

  sys_abort(reboot_flag, MM_PROC_NR, monitor_code, reboot_size);
  /* NOTREACHED */
}

/*=====================================================================*
 *			    do_svrctl				       *
 *=====================================================================*/
PUBLIC int do_svrctl()
{
  int req;
  vir_bytes ptr;

  req = svrctl_req;
  ptr = (vir_bytes) svrctl_argp;

  /* Is the request for the kernel? */
  if (((req >> 8) & 0xFF) == 'S') {
	return(sys_sysctl(who, req, mp->mp_effuid == SUPER_USER, ptr));
  }

  switch(req) {
  case MMSIGNON: {
	/* A user process becomes a task.  Simulate an exit by
	 * releasing a waiting parent and disinheriting children.
	 */
	struct mproc *rmp;
	pid_t pidarg;

	if (mp->mp_effuid != SUPER_USER) return(EPERM);

	rmp = &mproc[mp->mp_parent];
	tell_fs(EXIT, who, 0, 0);

	pidarg = rmp->mp_wpid;
	if ((rmp->mp_flags & WAITING) && (pidarg == -1
		|| pidarg == mp->mp_pid || -pidarg == mp->mp_procgrp))
	{
		/* Wake up the parent. */
		rmp->reply_res2 = 0;
		setreply(mp->mp_parent, mp->mp_pid);
		rmp->mp_flags &= ~WAITING;
	}

	/* Disinherit children. */
	for (rmp = &mproc[0]; rmp < &mproc[NR_PROCS]; rmp++) {
		if (rmp->mp_flags & IN_USE && rmp->mp_parent == who) {
			rmp->mp_parent = INIT_PROC_NR;
		}
	}

	/* Become like MM and FS. */
	mp->mp_pid = mp->mp_procgrp = 0;
	mp->mp_parent = 0;
	return(OK); }

  case MMSWAPON: {
	struct mmswapon swapon;

	if (mp->mp_effuid != SUPER_USER) return(EPERM);

	if (sys_copy(who, D, (phys_bytes) ptr,
		MM_PROC_NR, D, (phys_bytes) &swapon,
		(phys_bytes) sizeof(swapon)) != OK) return(EFAULT);

	return(swap_on(swapon.file, swapon.offset, swapon.size)); }

  case MMSWAPOFF: {
	if (mp->mp_effuid != SUPER_USER) return(EPERM);

	return(swap_off()); }

  default:
	return(EINVAL);
  }
}

