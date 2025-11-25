#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int child_stuff(){
  char *argv[] = {"/bin/sh", 0};
  if(execve("/bin/sh", argv, NULL) < 0)
    perror("execve");
  return 0;
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <program> \n", argv[0]);
    return 1;
  }
  printf("executing \"%s\"\n", argv[1]);
  //fork
  pid_t pid = fork();
  switch(pid){
  case -1:
    perror("[FATAL] Couldn't create child");
    return 1;
    break; //idk if this required after a return
  case 0:
    //child
    return child_stuff();
    break;
  default:
    //parent
    int wstatus;
    printf("waiting on: %d\n", pid);
    pid_t ret_val = wait(&wstatus);

    //exit handling
    printf("%d exited ", ret_val);
    if(WIFEXITED(wstatus))
      printf("with status %d", WEXITSTATUS(wstatus));
    fputs("\n", stdout);
    break;
  }
}
