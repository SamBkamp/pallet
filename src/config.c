#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "config.h"

//adapted from https://github.com/SamBkamp/cursed_config_parser



int read_file(char *file_data){
  struct stat sb;
  ssize_t bytes_read;
  int conf_fd = open("config.ini", O_RDONLY);
  
  if(conf_fd<0) return -1;
  if(fstat(conf_fd, &sb)<0 || sb.st_size >= FILE_BUFFER_SIZE) return 1;
  
  bytes_read = read(conf_fd, file_data, FILE_BUFFER_SIZE);
  if(bytes_read < 0) return -1;

  close(conf_fd);
  
  file_data[bytes_read] = 0;
  return bytes_read;
}

//takes a line src and returns a pointer to the value after the '='. puts a null terminator at the start of delimeter. Accounts for spaces either side of equals.
//eg: field = value -> field\0= value (where val points to the first char of the value ie. 'v' in this case)
//or filed=value -> field\0value 
char* split_line(char *src){
  char *line_end = src + strlen(src); //points to null terminator
  char *equals_pos;
  for(equals_pos = src; equals_pos < line_end && *equals_pos != '='; equals_pos++){}
  if(equals_pos == line_end)
    return equals_pos;
  if(*(equals_pos-1) == ' ')
    *(equals_pos-1) = 0;
  else
    *equals_pos = 0;
  
  equals_pos++;
  while(*equals_pos == ' ')
    equals_pos++;
  return equals_pos;
}

//parses the UID from the format user:UID where UID is a base 10 number using strtol
ssize_t get_uid(char *user){
  int i = 0;
  ssize_t ret;
  char *endptr;
  for(; user[i] != 0 && user[i]!=':'; i++){}
  if(user[i] == 0) return -1;
  user[i] = 0;
  ret = strtol(&user[i+1], &endptr, 10);
  if(&user[i+1] != endptr)
    return ret;

  perror("strtol");
  return -1;
}


int open_config(config *cfg){
  char file_data[FILE_BUFFER_SIZE];
  
  if(read_file(file_data)<0){
    perror("can't read config file");
    return -1;
  }
  
  char *tok = strtok(file_data, "\n");
  while(tok != NULL){
    char *val = split_line(tok);
    if(*tok != '#' && *val != 0){
      if(strcmp(tok, "user")==0){
        cfg->user = malloc(strlen(val));
        strcpy(cfg->user, val);
        cfg->uid = get_uid(cfg->user);
      }else if(strcmp(tok, "group")==0){
        cfg->group = malloc(strlen(val));
        strcpy(cfg->group, val);
        cfg->gid = get_uid(cfg->group);
      }else{
        printf("unknown directive %s\n", tok);
        return -1;
      }
    }
    tok = strtok(NULL, "\n");
  }

  return 0;
}
