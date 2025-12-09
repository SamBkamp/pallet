#ifndef PALLET_CONFIG
#define PALLET_CONFIG

#include <stdint.h>
#define FILE_BUFFER_SIZE 1024

typedef struct{
  char *user;
  uint32_t uid;
  char *group;
  uint32_t gid;
}config;

int open_config(config *cfg);

#endif
