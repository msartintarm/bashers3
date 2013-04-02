#include "raidZero.h"
#include "raidFour.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int raid_level;
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
  switch(raid_level) {
    
  case 0:
    zeroInit(the_array, strip_, disk_);
    break;
  case 4:
    fourInit(the_array, strip_, disk_);
    break;
  }
}

void raid_disk_array_read(int block_num, int block_size) {

  switch(raid_level) {
	
  case 0:
    printf("Raid 0 read result: %d\n", zeroRead(block_size, block_num));
    break;
  case 4:
    printf("Raid 4 read result: %d\n", fourRead(block_size, block_num));
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
	   zeroWrite(block_size, block_num, value));
    break;
  case 4:
    printf("Raid 4 write result: %d\n", 
	   fourWrite(block_size, block_num, value));
    break;
  default:
    ;
    break;
  }

}

void raid_disk_fail(int disk_num) {
  int rc;
  switch(raid_level) {
  case 0:
    rc = zeroFail(disk_num);
    if(rc == 0){
      printf("Disk %d failure.\n", disk_num);
    }
    else {
      printf("Error failing disk %d\n", disk_num);
    }
    break;
  case 4:
    rc = fourFail(disk_num);
    if(rc == 0){
      printf("Disk %d failure.\n", disk_num);
    }
    else {
      printf("Error failing disk %d\n", disk_num);
    }
    break;
  default:
    ;
    break;
  }
 
}

void raid_disk_recover(int disk_num) {

  printf("Disk %d recovery.\n", disk_num);
}


