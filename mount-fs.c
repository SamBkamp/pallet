#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <stdint.h>
#include <string.h>

typedef struct{
  uint8_t mount;
}settings;

typedef struct{
  char *host_point;
  char *fs_point;
}mount_point;

int main(int argc, char* argv[]){
  settings settings = {0}; //i think these are in diff namespaces
  mount_point mntpts[] = {{"/lib", "fs/lib"},
                           {"/lib32", "fs/lib32"},
                           {"/lib64", "fs/lib64"},
                           {"/bin", "fs/bin"}};
  for(uint8_t i = 1; i < argc; i++){
    if(strcmp(argv[i], "--unmount") == 0){
      settings.mount = 1;
      break;
    }
  }
  if(settings.mount == 0){
    for(int i = 0; i < sizeof(mntpts); i++)
      mount(mntpts[i].host_point, mntpts[i].fs_point, "ext4", MS_BIND, NULL);
    
  }else{
    for(int i = 0; i < sizeof(mntpts); i++)
      umount(mntpts[i].fs_point);
  }
  
}
