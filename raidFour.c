#include "raidFour.h"
#include <stdio.h>
#include <string.h>


int strip;
int disk;
char buffer[BLOCK_SIZE];

/*RAID 4
*
  Striped in strip size units across the first n-1 disks
   Disk n should receive the parity. 

  For 4 disks and strip size 3, 
   disk 0 gets blocks 0-2 and 9-11, 
   disk 1 gets blocks 3-5 and 12-14, 
   disk 2 gets blocks 6-8 and 15-17. 

  The parity disk gets the byte-wise XOR of the n-1 data disks. 
  
  In this case, block 0 XOR block 3 XOR block 6, 
                block 1 XOR block 4 XOR block 7, 
                block 2 XOR block 5 XOR block 8 etc.*/

static disk_array_t disk_arr;
static int num_disks;
static int strip_size;

void fourInit(disk_array_t da, int num_disks_,
	      int strip_size_) {
  disk_arr = da;
  num_disks = num_disks_;
  strip_size = strip_size_;
}

/**
 * yeah I went there. Strip those disks!
 */
static int stripper(int size, int lba, char* value, short isWrite) {

  int block_offset = lba % strip_size;
  int disk_num = (lba - block_offset) / (num_disks - 1);
  while(disk_num >= (num_disks - 1)) {
    block_offset += (num_disks - 1) * strip_size;
    disk_num -= (num_disks - 1);
  }
  // Starting disk number and offset found.

  int i;
  for(i = 0; i < strip_size; ++i) {
    if(isWrite == 1){
      disk_array_write(disk_arr, disk_num, block_offset, value);
      printf("Writing [disk,block]: [%d,%d]\n", disk_num, block_offset);
    }
    //Read operation
    else {
      disk_array_read(disk_arr, disk_num, block_offset, buffer);
      printf("Reading [disk,block]: [%d,%d]\n", disk_num, block_offset);
    }
    if(++block_offset % (num_disks - 1) * strip_size == 0) {
      block_offset -= (num_disks - 1) * strip_size;
    }
  }
  
  return 0;
}

int fourRead(int size, int lba) {
  return stripper(size, lba, NULL, 0);
}

int fourWrite(int size, int lba, char* value) {
  return stripper(size, lba, value, 1);
}

int fourFail(int failed_disk) {
  return disk_array_fail_disk(disk_arr, failed_disk);
}


//	      disk_array_recover_disk(da,failed_disk);
//	      
//	    if (disk_array_read(da, disk, block, buffer) == 0) {
//	    disk_array_write(da, disk, block, buffer);
