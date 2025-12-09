#include <string.h>

#include "config.h"
#include "ipc.h"

/*
------------------------------------
            WARNING!!
    Do not send this serialized
    data over a network socket
    or to another machine in any
    way. Different machines may
    be running on different
    endian-ness and deserializing
    numbers will break and be
    decoded incorrectly.
    
------------------------------------
 */

//serializes executable name and some config data into contigious memory. Data elements can be both variable length and fixed length. Variable length data must be null terminated and should be ascii strings.
//returns size of buffer to send
ssize_t serialize(char *mem, char *program, config *cfg){
  //program
  ssize_t idx = strlen(program)+1;
  memcpy(mem, program, idx);

  //uid (as a uint32_t)
  memcpy((mem+idx), &(cfg->uid), sizeof(uint32_t));
  idx+=sizeof(uint32_t);
  
  return idx;
}


//deserializes data serialized from serialize()
void deserialize(char *mem, child_cfg *cfg){
  cfg->program = mem;
  cfg->uid = *((uint32_t *)(mem+strlen(cfg->program)+1));
  //uid offset is start + strlen of program + 1 (for null term.). These bytes will point to the uint32, this is then dereferenced as a uint32_t
  return;
}
