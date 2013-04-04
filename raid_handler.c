#include "raidZero.h"
#include "raidTen.h"
#include "raidFour.h"
#include "raidFive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int verbose;
int raid_level;
int _buff[BLOCK_SIZE / (sizeof(int) / sizeof(char))];

void printd(char* str) {
  if(verbose) printf(str);
}
void printd1(char* str, int arg1) {
  if(verbose) printf(str, arg1);
}
void printd2(char* str, int arg1, int arg2) {
  if(verbose) printf(str, arg1, arg2);
}

char* trimNewline(char* str) {
  
  char* end = str + strlen(str) - 1;
  while(end >= str && (*end) == '\n') {
    end--; 
  }
  // Write new null terminator
  *(end+1) = '\0';
  return str;
}

static disk_array_t _da;

void raid_init(int the_level, 
			   int strip_size, 
			   int num_disks,
			   int disk_size) {

  _da = disk_array_create("myvirtualdisk", num_disks, disk_size);
  if(!_da) {
	fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
	exit(1);
  }

  raid_level = the_level;

  switch(raid_level) {
    
  case 0:
    zeroInit(_da, strip_size, num_disks);
    break;
  case 10:
    tenInit(_da, strip_size, num_disks, disk_size);
    break;
  case 4:
    fourInit(_da, strip_size, num_disks, disk_size);
    break;
  case 5:
    fiveInit(the_array, strip_size, num_disks, disk_size);
    break;
  }
}

void raid_disk_array_read(int block_num, int block_size) {

  switch(raid_level) {
	
  case 0:
    zeroRead(block_size, block_num);
    break;
  case 10:
    tenRead(block_size, block_num);
    break;
  case 4:
    fourRead(block_size, block_num);
    break;
  case 5:
    fiveRead(block_size, block_num);
    break;
  default:
    ;
    break;
  }


}

int ii;

void raid_disk_array_write(int block_num, int block_size, int value) {
  
  int numWrites = 0;
  for(ii = 0; ii < sizeof(_buff) / sizeof(int); ++ii) {
	_buff[ii] = value;
	numWrites ++;
  }

  switch(raid_level) {
  case 0:
	  zeroWrite(block_size, block_num, (char*)_buff);
    break;
  case 10:
    tenWrite(block_size, block_num, (char*)_buff);
    break;
  case 4:
	  fourWrite(block_size, block_num, (char*)_buff);
    break;
  case 5:
    fiveWrite(block_size, block_num, (char*)_buff);
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
    break;
  case 10:
    rc = tenFail(disk_num);
    break;
  case 4:
    rc = fourFail(disk_num);
    break;
  case 5:
    rc = fiveFail(disk_num);
    break;
  default:
    rc = 1;
    break;
  }
  if(rc != 0){
	printf("Error failing disk %d\n", disk_num);
  }
}

void raid_disk_recover(int disk_num) {

  int rc;
  switch(raid_level) {
  case 0:
    rc = zeroRecover(disk_num);
    break;
  case 10:
    rc = tenRecover(disk_num);
    break;
  case 4:
    rc = fourRecover(disk_num);
    break;
  case 5:
    rc = fiveRecover(disk_num);
    break;
  default:
    rc = 1;
    break;
  }
  if(rc != 0){
	printd1("Error recovering disk %d\n", disk_num);
  }
}

void raid_cleanup() {


  switch(raid_level) {
  case 4:
    fourCleanup();
    break;
  case 0:
    zeroCleanup();
    break;
  case 10:
    tenCleanup();
    break;
  case 5:
    fiveCleanup();
    break;
  default:
    break;
  }

  disk_array_print_stats(_da);
  disk_array_close(_da);

}
