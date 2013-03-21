#include "raidZero.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int raid_level;
disk_array_t da;
int disk;
int strip;

char* trimNewline(char* str) {
  
  char* end = str + strlen(str) - 1;
  while(end >= str && (*end) == '\n') {
	end--; 
  }
  // Write new null terminator
  *(end+1) = '\0';
  return str;
}


void raid_init(disk_array_t the_array, int the_level, int strip_, int disk_) {
  raid_level = the_level;
  da = the_array;
  disk = disk_;
  strip = strip_;
}

void raid_disk_array_read(int block_num, int block_size) {

  switch(raid_level) {
	
  case 0:
	printf("Raid 0 read result: %d\n", zeroRead(da, block_size, block_num));
	break;
  default:
	;
	break;
  }


}

void raid_disk_array_write(int block_num, int block_size, char* value) {
  
  trimNewline(value);
  switch(raid_level) {
  case 0:
	printf("Raid 0 write result: %d\n", 
		   zeroWrite(da, block_size, block_num, value));
	break;
  default:
	;
	break;
  }

}

void raid_disk_fail(int disk_num) {

 printf("Disk %d failure.\n", disk_num);
}

void raid_disk_recover(int disk_num) {

  printf("Disk %d recovery.\n", disk_num);
}


