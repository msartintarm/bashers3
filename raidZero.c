#include "raidZero.h"
#include <stdio.h>
#include <string.h>

/* TODO We need to do some bookeeping right away.
 * Setup the disk structure before hand which will 
 * provide the mapping of the LBA to the proper (disk,block) address
 * Then read/write can just iterate without knowing anything about disk structure
 */ 

int strip;
int disk;
char buffer[BLOCK_SIZE];

/*RAID 0
*
 Blocks should be striped across disks, starting with the lowest numbered disk and increasing.
 Each disk gets a strip of blocks, which is one or more   blocks.
 For example, with 3 disks and strip size of 2,
 disk 0 gets blocks 0-1 and 6-7,
 disk 1 gets blocks 2-3 and 8-9, &
 disk 3 gets blocks 4-5 and 10-11.
*/

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

int zeroRead(disk_array_t da, int size, int lba) {
  int rc = 0;
  rc = stripper(da, size, lba, NULL, 1);
  return rc;
}

int zeroWrite(disk_array_t da, int size, int lba, char* value) {
  int rc = 0;
  rc = stripper(da, size, lba, value, 1);
  return rc;
}

int zeroFail(disk_array_t da, int failed_disk) {
  int rc = 0;
  rc = disk_array_fail_disk(da,failed_disk);
  return rc;
}


//	      disk_array_recover_disk(da,failed_disk);
//	      
//	    if (disk_array_read(da, disk, block, buffer) == 0) {
//	    disk_array_write(da, disk, block, buffer);
