#ifndef PALLET_IPC
#define PALLET_IPC
#include <sys/types.h>
#include <stdint.h>

typedef struct{
  char *program;
  uint32_t uid;
  uint32_t gid;
}child_cfg; //config data the child needs

ssize_t serialize(char *mem, char *program, config *cfg);
void deserialize(char *mem, child_cfg *cfg);
#endif
