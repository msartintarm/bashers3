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

/**
 * yeah I went there. Strip those disks!
 */
static int stripper(disk_array_t da, int size, int lba, char* value, short isWrite) {




  int rc = 0;
  short startFound = 0;
  int diskIndex  = 0;
  int stripIndex = 0; 
  int blockIndex = 0; //index of block within disk
  int multiple   = 0; //used to move blocks to next set of strips
  int i;
  //loop until address reached
  for(i = 0; i < size; i++){
    if(i == lba){
      startFound = 1;
    }
    if(startFound == 1){
      if(isWrite == 1){
	      disk_array_write(da, diskIndex, blockIndex, value);
        printf("Writing [disk,block]: [%d,%d]\n", diskIndex, blockIndex);
      }
      //Read operation
      else {
        disk_array_read(da, diskIndex, blockIndex, buffer);
        printf("Reading [disk,block]: [%d,%d]\n", diskIndex, blockIndex);
      }
    }
    
    //if reached end of strip change disk
    if((strip-1) == stripIndex){
      //if reached last disk move back to disk 0
      if(diskIndex == (disk-1)){
        diskIndex = 0;
        multiple += strip;
        stripIndex = 0;
      }
      //not the last disk
      else {
        //blockIndex = multiple;
        diskIndex++;
        stripIndex = 0;
      }
      blockIndex = multiple; //reset to block to strip multiple
      
    }
    //move to next block/strip
    else {
      blockIndex++;
      stripIndex++;
    }
    
  }
  diskIndex = diskIndex % disk;
  //printf("Found: [%d,%d]\n", diskIndex, blockIndex);
  
  return rc;
}

int fourRead(disk_array_t da, int size, int lba) {
  int rc = 0;
  rc = stripper(da, size, lba, NULL, 1);
  return rc;
}

int fourWrite(disk_array_t da, int size, int lba, char* value) {
  int rc = 0;
  rc = stripper(da, size, lba, value, 1);
  return rc;
}

int fourFail(disk_array_t da, int failed_disk) {
  int rc = 0;
  rc = disk_array_fail_disk(da,failed_disk);
  return rc;
}


//	      disk_array_recover_disk(da,failed_disk);
//	      
//	    if (disk_array_read(da, disk, block, buffer) == 0) {
//	    disk_array_write(da, disk, block, buffer);
