#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <linux/sched.h>    /* Definition of struct clone_args */
#include <sched.h>          /* Definition of CLONE_* constants */

#include <sys/syscall.h>    /* Definition of SYS_* constants */
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <sys/types.h> /* for mkfifo */
#include <sys/stat.h>  /* for mkfifo & stat */

#include "src/config.h"
#include "src/ipc.h"

//error that perrors then quits if return val of a function (i) is < 0
#define LOG_QUIT_ON_ERR(i, s) {if(i <  0){perror(s); _exit(1);}}

typedef struct{
  char *host_point;
  char *fs_point;
}mount_point;

//this is the child function. IT RUNS AS A SEPERATE PROGRAM. Consider this _start(). IMPORTANT: this function has nowhere to return to. returning will probably segfault. You must _exit();
void child_main(){
  child_cfg cfg = {0};
  char root_dir[1024];
  char message[1024];
  char *argv[2] = {0};
  int fd = open("./dtx.fifo", O_RDONLY); //this will block as long as main thread doesn't open for write
  int bytes_read = read(fd, message, 1023);
  mount_point mntpts[] = {{"/lib", "fs/lib"},
                          {"/lib32", "fs/lib32"},
                          {"/lib64", "fs/lib64"},
                          {"/bin", "fs/bin"}};

  //read and process data from pipe
  message[bytes_read] = 0;
  deserialize(message, &cfg);
  argv[0] = cfg.program;

  //privatise "/" mount in child mount ns
  LOG_QUIT_ON_ERR(mount("ignored", "/", "ignored", MS_PRIVATE|MS_REC, "ignored"),"couldn't private / mount");

  //bind mount system files
  for(int i = 0; i < sizeof(mntpts); i++)
    mount(mntpts[i].host_point, mntpts[i].fs_point, NULL, MS_BIND | MS_PRIVATE | MS_REC, NULL);


  getcwd(root_dir, 1010);
  strcat(root_dir, "/fs");

  if(chdir(root_dir) < 0) perror("chdir");
  if(chroot(root_dir) < 0) perror("chroot");

  //should we use access() instead?
  //surely I don't gotta nest 3 fucking ifs for the behaviour I want
  //god bless the cpu branch predictor
  struct stat statbuf; //uneeded for now
  if(stat("/proc/cpuinfo", &statbuf) < 0){
    if(errno == ENOENT){
      LOG_QUIT_ON_ERR(mount("proc", "/proc", "proc", 0, NULL), "couldn't mount proc fs");
    }else
      perror("stat");
  }

  if(sethostname("pallet", 6)<0) perror("sethostname");
  if(setgid(1002)<0) perror("setgid");
  if(setuid(cfg.uid)<0) perror("setuid");
  if(chdir("/home/dummy") < 0) perror("local chdir");

  if(execve(argv[0], argv, NULL) < 0) perror("execve");

  _exit(0);
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <program> \n", argv[0]);
    return 1;
  }
  umask(000);
  config cfg;
  long page_size = (sysconf(_SC_PAGE_SIZE)*2);
  //childs stack
  char *stack = mmap(NULL,
                     page_size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1,
                     0);
  if(stack == MAP_FAILED){
    perror("[FATAL] mmap");
    return 1;
  }
  LOG_QUIT_ON_ERR(open_config(&cfg), "open config");
  //could lead to a potential bug where the file exists but we don't have adequate perms to write/read to/from. Very random and probably won't happen but maybe
  int mk_fifo = mkfifo("./dtx.fifo", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH);
  if(mk_fifo < 0 && errno != EEXIST){
    perror("[FATAL] mkfifo");
    return 1;
  }

  char *top_of_stack = stack+page_size;
  uint64_t *sp = (uint64_t *)top_of_stack;
  //seed the stack with our function value that will be popped and ret'd
  *(--sp) = (uint64_t)child_main;

  pid_t tid_array[] = {1};

  struct clone_args cl_args = {
    .flags = CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS,
    .exit_signal = SIGCHLD,
    .stack = (uint64_t)(stack-8), //stack needs to point to the low boundary
    .stack_size = page_size,
    .set_tid = (uint64_t)&tid_array,
    .set_tid_size = 1
  };

  long pid = syscall(SYS_clone3, &cl_args, sizeof(cl_args));
  //no need for child case as it rets directly into our child function
  switch(pid){
  case -1:
    perror("[FATAL] Couldn't create child");
    return 1;
    break; //idk if this required after a return
  case 0:
    break;
  default:
    //parent
    ;
    char message[1024];
    int fd = open("./dtx.fifo", O_WRONLY);
    siginfo_t wstatus = {0};
    ssize_t size = serialize(message, argv[1], &cfg);

    write(fd, message, size);
    printf("%ld is running %s\n---------------------\n", pid, argv[1]);
    pid_t ret_val = waitid(P_PID, pid, &wstatus, WEXITED);
    if(ret_val < 0){
      perror("[FATAL] waitid");
      break;
    }
    //exit handling
    printf("-----------------\n%ld exited with status %d: %s\n", pid, wstatus.si_status, strsignal(wstatus.si_status));

    break;
  }
}
