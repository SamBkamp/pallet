#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int child_stuff(){
  sleep(2);
  printf("eyyy im processing ova here!\n");
  sleep(1);
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
    printf("waiting on: %d\n", pid);
    pid_t ret_val = waitpid(pid, NULL, 0);
    printf("%d exited\n", ret_val);
    break;
  }
}
