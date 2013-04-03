#include "raidTen.h"
#include "raid_handler.h"
#include <stdio.h>
#include <string.h>

/* TODO We need to do some bookeeping right away.
 * Setup the disk structure before hand which will 
 * provide the mapping of the LBA to the proper (disk,block) address
 * Then read/write can just iterate without knowing anything about disk structure
 */ 

int _strip;
int _disk;
char buffer[BLOCK_SIZE];
static disk_array_t _da;

void tenInit(disk_array_t da, int strip, int disk) {
  _da = da;
  _strip = strip;
  _disk = disk;
}

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
 * stripping with mirrors baby.
 */
static int stripper(int size, int lba, char* value, short isWrite) {
  int rc = 0;
  short startFound = 0;
  int diskIndex  = 0;
  int stripIndex = 0; 
  int blockIndex = 0; //index of block within disk
  int multiple   = 0; //used to move blocks to next set of strips
  int i;
  //loop until address reached
  for(i = 0; i < size + lba; i++){
    if(i == lba){
      startFound = 1;
    }
    if(startFound == 1){
      if(isWrite == 1){
	      disk_array_write(_da, diskIndex, blockIndex, value);
	      disk_array_write(_da, diskIndex+1, blockIndex, value); //mirror disk
	      /**if(verbose){
            printf("Writing [disk,block]: [%d,%d]\n", diskIndex, blockIndex);
           }**/
      }
      //Read operation
      else {
        disk_array_read(_da, diskIndex, blockIndex, buffer);
        //If that disk is failed try its mirror
        if(strcmp(buffer, "ERROR") == 0) { //is this right?
          disk_array_read(_da, diskIndex+1, blockIndex, buffer);
          /**if(verbose){
              printf("Error reading disk %d. Reading from mirror disk.\nReading [disk,block]: [%d,%d]\n", diskIndex, diskIndex+1, blockIndex);
            }**/
        }
        /**else {
          printf("Reading [disk,block]: [%d,%d]\n", diskIndex, blockIndex);
        }**/
      }
    }
    
    //if reached end of strip change disk
    if((_strip-1) == stripIndex){
      //if reached last disk move back to disk 0
      if(diskIndex == (_disk-1)){
        diskIndex = 0;
        multiple += _strip;
        stripIndex = 0;
      }
      //not the last disk
      else {
        //blockIndex = multiple;
        diskIndex += 2; //need to skip past mirror disk
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
  diskIndex = diskIndex % _disk;
  //printf("Found: [%d,%d]\n", diskIndex, blockIndex);
  
  return rc;
}


int tenRead(int size, int lba) {
  int rc = 0;
  rc = stripper(size, lba, NULL, 0);
  return rc;
}

int tenWrite(int size, int lba, char* value) {
  int rc = 0;
  rc = stripper(size, lba, value, 1);
  return rc;
}

int tenFail(int failed_disk) {
  int rc = 0;
  rc = disk_array_fail_disk(_da, failed_disk);
  return rc;
}

/**
/call function to zero out disk
/next copy mirrored disk to the new disk
/return -1 if both copys have failed.
**/
int tenRecover(int new_disk) {
  int rc = 0;
  //find which pair to replace
  short even = -1;
  if((new_disk % 2) == 0) {
    even = 1; //recover even/left disk
  } 
  rc = disk_array_recover_disk(_da,new_disk);
  if(rc == 0) {
    int i;
    int blocks = disk_array_nblocks(_da)/_disk; //default case where blocks split evenly
    //do the blocks split evenly?
    int rem_blocks = disk_array_nblocks(_da) % _disk;
    //if true then there is a remainder of blocks on the last disk
    //write to portion ignoring fragment at the end
    if(rem_blocks != 0 && ((new_disk == _disk-1) || (new_disk == _disk-2))) {
      //printf("remaining portion: %d %d %d\n", rem_blocks, _disk, new_disk);
      for(i = 0; i < rem_blocks; i++) {
        disk_array_read(_da, new_disk+even, i, buffer); 
        disk_array_write(_da, new_disk, i, buffer);
      }
    }
    //standard case
    else {
      //printf("normal disk: rem%d block%d\n", rem_blocks, blocks);
      for(i = 0; i < blocks; i++) {
        disk_array_read(_da, new_disk+even, i, buffer); 
        disk_array_write(_da, new_disk, i, buffer);
      }
    }
    
  }
  return rc;
}

