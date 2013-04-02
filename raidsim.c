#include "raid_handler.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int verbose;

int main(int argc, char *argv[]){

  const char usage[] = "usage: raidsim -level int -strip int -disks int \
-size int -trace filename -verbose(optional)\n";
  int level = 0;
  int strip = 0;
  int disks = 0;
  int size = 0;
  char* trace;
  verbose = 0;

  
  if(argc == 12){
    int is_verbose = strcmp(argv[11],"-verbose");
    if(is_verbose == 0){
      verbose = 1;
    }else{
      fprintf(stderr, usage);
      exit(0);
    }
  }
  
  if(argc == 12 || argc == 11){
    int i;
    for(i = 1; i <= 9; i+=2){
      int is_level = strcmp(argv[i], "-level");
      int is_strip = strcmp(argv[i], "-strip");
      int is_disks = strcmp(argv[i], "-disks");
      int is_size = strcmp(argv[i], "-size");      
      int is_trace = strcmp(argv[i], "-trace");

      if(is_level==0){
        level = atoi(argv[i+1]);
        if(!(level == 0 || level == 4 || level == 5 || level == 10)){
          fprintf(stderr, "only raid levels 0, 10, 4, and 5 available\n");
          exit(0);
        }
      }else if(is_strip==0){
        strip = atoi(argv[i+1]);
      }else if(is_disks==0){
        disks = atoi(argv[i+1]);
      }else if(is_size==0){
        size = atoi(argv[i+1]);
      }else if(is_trace==0){
        trace = argv[i+1];
      }else{
        fprintf(stderr, usage);
        exit(0);
      }
    }
  
  }else{
    fprintf(stderr, usage);
    exit(0);
  }
  
  if(level == 10 && disks % 2 != 0){
    fprintf(stderr, "number of disks must be even for raid level 10\n");
    exit(0);
  }
  
  if((level == 4 || level == 5) && disks < 2){
    fprintf(stderr, "must have more than 2 disks for raid level %d\n", level);
    exit(0);
  }

  // Input params have passed syntax checking - now init disks and RAID.
  
	disk_array_t da = disk_array_create("myvirtualdisk", disks, size);
	if(!da) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}

	raid_init(da, level, strip, disks);

  FILE* trace_file = fopen(trace, "r");
  if(trace_file == NULL){
    fprintf(stderr, "bad trace file\n");
    exit(0);
  }

  char* one_line = malloc(513 * sizeof(char));

  while(fgets(one_line, 513, trace_file) != NULL) {

	char* first_word = strtok(one_line, " ");

	if(!strcmp(first_word, "READ")) {
	  int block_num = atoi(strtok(NULL, " "));
	  int block_size = atoi(strtok(NULL, " "));
	  raid_disk_array_read(block_num, block_size);

	} else if(!strcmp(first_word, "WRITE")) {
	  // Value is 4 bytes : we assume 4 chars
	  int block_num = atoi(strtok(NULL, " "));
	  int block_size = atoi(strtok(NULL, " "));
	  int value = atoi(strtok(NULL, " "));
	  printd1(" int value to write is %d", value);
	  raid_disk_array_write(block_num, block_size, value);

	} else if(!strcmp(first_word, "FAIL")) {
	  int disk_num = atoi(strtok(NULL, " "));
	  raid_disk_fail(disk_num);
	  
	} else if(!strcmp(first_word, "RECOVER")) {
	  int disk_num = atoi(strtok(NULL, " "));
	  raid_disk_recover(disk_num);
	  
	} else if(!strcmp(first_word, "END")) {
	  //	  disk_array_print_stats();
	  break;
	}
  }
  
  free(one_line);

  fclose(trace_file);
  disk_array_close(da);
  return 0;

}
