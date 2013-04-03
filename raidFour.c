#include "raidFour.h"
#include "raid_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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


static int i;
static disk_array_t disk_arr;
static int num_disks;   // Does NOT include parity
// Note that the disk count is the same as the parity ID
#define parity_disk num_disks
static int strip_size;
static int disk_size;
static char buffer[BLOCK_SIZE];
static int parityBuff[BLOCK_SIZE];
static int* disk_active; // 0 denotes the disk is failed

void fourInit(disk_array_t da, int strip_size_,
	      int num_disks_, int disk_size_) {
  disk_arr = da;
  num_disks = num_disks_ - 1;
  strip_size = strip_size_;
  disk_size = disk_size_;
  disk_active = malloc(num_disks * sizeof(int));
  // All disks start out as active (not failed)
  for(i = 0; i < num_disks; ++i) {
    disk_active[i] = 1;
  }

  // One thing we do NOT do here: initialize parity disk.
  //  Why? Because all data (and hence parity) is 0 to 
  //  startout with. No need.
}

/**
 * yeah I went there. Strip those disks!
 */
static int stripper(int size, int lba, char* value, short isWrite) {

  // Find starting disk number and offset.
  int block_offset = lba % strip_size;
  int disk_num = (lba - block_offset) / num_disks;
  while(disk_num >= (num_disks)) {
    block_offset += strip_size;
    disk_num -= (num_disks);
  }

  printd1(" Block address: %d\n", lba);
  printd2(" Starting block offset: %d, disk Num: %d\n", block_offset, disk_num);

  for(i = 0; i < size; ++i) {

    if(isWrite == 1){
      if(!disk_active[disk_num]) {
	printf("ERROR"); 
	continue;
      }
      // First read the old data and parity
      disk_array_read(disk_arr, disk_num, block_offset, buffer);
      disk_array_read(disk_arr, parity_disk, block_offset, (char*)parityBuff);

      parityBuff[0] ^= *(int*)buffer; // Remove old data from parity
      parityBuff[0] ^= *(int*)value; // Add new data to parity

      disk_array_write(disk_arr, disk_num, block_offset, value);
      disk_array_write(disk_arr, parity_disk, block_offset, (char*)parityBuff);

    } else { //Read operation
      if(!disk_active[disk_num]) {
	printd1("Reconstructing data for disk %d from parity.\n", disk_num);
	disk_array_read(disk_arr, parity_disk, block_offset, (char*)parityBuff);
	int j;
	for(j = 0; j < num_disks; ++j) {
	  if(!disk_active[disk_num]) continue;
	    disk_array_read(disk_arr, disk_num, block_offset, buffer);
	    parityBuff[0] ^= *(int*)buffer;
      printf("%d\n", *((int*)parityBuff));
	}
      } else {
	disk_array_read(disk_arr, disk_num, block_offset, buffer);
	printf("%d\n", *((int*)buffer));
      }
    }
    // Compute new index
    if(++block_offset % strip_size == 0) {
      block_offset -= strip_size;
	  disk_num += 1;
	  disk_num %= num_disks;
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
  disk_active[failed_disk] = 0;
  return disk_array_fail_disk(disk_arr, failed_disk);
}

int fourRecover(int recovered_disk) {
  disk_active[recovered_disk] = 1;
  return disk_array_recover_disk(disk_arr, recovered_disk);
}

