#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/sched.h>    /* Definition of struct clone_args */
#include <sched.h>          /* Definition of CLONE_* constants */
#include <sys/syscall.h>    /* Definition of SYS_* constants */
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>

#define STACK_SIZE 8192

int child_stuff(){
  char *argv[] = {"/bin/bash", 0};
  //sethostname("pallet", 6);
  if(execve("/bin/bash", argv, NULL) < 0)
    perror("execve");
  _exit(0);
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <program> \n", argv[0]);
    return 1;
  }
  printf("executing \"%s\"\n", argv[1]);

  /*
  //childs stack
  char *stack = mmap(NULL,
                     STACK_SIZE,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANON | MAP_GROWSDOWN,
                     -1,
                     0);
  if(stack == MAP_FAILED){
    perror("[FATAL] mmap");
    return 1;
  }
  */
  //pid_t tid_array[] = {1};
  struct clone_args cl_args = {
    .flags = CLONE_NEWUTS,
    .exit_signal = SIGCHLD
  };
  //seed the stack with our function value that will be popped and ret'd
  //*top_of_stack = (uint64_t)child_stuff;

  long pid = syscall(SYS_clone3, &cl_args, sizeof(cl_args));
  //no need for child case as it rets directly into our child function
  switch(pid){
  case -1:
    perror("[FATAL] Couldn't create child");
    return 1;
    break; //idk if this required after a return
  case 0:
    return child_stuff();
  default:
    //parent
    ;
    siginfo_t wstatus = {0};
    printf("waiting on: %ld\n", pid);
    pid_t ret_val = waitid(P_PID, pid, &wstatus, WEXITED);
    if(ret_val < 0){
      perror("[FATAL] waitid");
      break;
    }
    //exit handling
    printf("%ld exited with status %d: %s\n", pid, wstatus.si_status, strsignal(wstatus.si_status));
    break;
  }
}
