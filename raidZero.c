#include "raidZero.h"
#include "raid_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* TODO We need to do some bookeeping right away.
 * Setup the disk structure before hand which will 
 * provide the mapping of the LBA to the proper (disk,block) address
 * Then read/write can just iterate without knowing anything about disk structure
 */ 

int _strip;
int _disk;
char buffer[BLOCK_SIZE];
static disk_array_t _da;
static int* disk_active; // 0 denotes the disk is failed

void zeroInit(disk_array_t da, int strip, int disk) {
  _da = da;
  _strip = strip;
  _disk = disk;
  int i;
  disk_active = malloc(_disk-1 * sizeof(int));
  // All disks start out as active (not failed)
  for(i = 0; i < _disk-1; ++i) {
    disk_active[i] = 1;
  }
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
 * yeah I went there. Strip those disks!
 */
static int stripper(int size, int lba, char* value, short isWrite) {
  int rc = 0;
  short startFound = 0;
  int diskIndex  = 0;
  int stripIndex = 0; 
  int blockIndex = 0; //index of block within disk
  int multiple   = 0; //used to move blocks to next set of strips
  int i;
  short write_error = 0;
  //loop until address reached
  for(i = 0; i < size + lba; i++){
    if(i == lba){
      startFound = 1;
    }
    if(startFound == 1){
      if(isWrite == 1){
        if(!disk_active[diskIndex]) {
          write_error = 1;
        }
        else {
          if(!disk_active[diskIndex]) {
            printf("ERROR \n");
          }
          else {
	          disk_array_write(_da, diskIndex, blockIndex, value);
            //printf("Writing [disk,block]: [%d,%d]\n", diskIndex, blockIndex);
          }
        }
      }
      //Read operation
      else {
        disk_array_read(_da, diskIndex, blockIndex, buffer);
	      printf("%d\n", *((int*)buffer));
        //printf("Reading [disk,block]: [%d,%d]\n", diskIndex, blockIndex);
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
  diskIndex = diskIndex % _disk;
  if(write_error){
    printf("ERROR \n");
    write_error = 0;
  }
  //printf("Found: [%d,%d]\n", diskIndex, blockIndex);
  
  return rc;
}

int zeroRead(int size, int lba) {
  int rc = 0;
  rc = stripper(size, lba, NULL, 0);
  return rc;
}

int zeroWrite(int size, int lba, char* value) {
  int rc = 0;
  rc = stripper(size, lba, value, 1);
  return rc;
}

int zeroFail(int failed_disk) {
  int rc = 0;
  disk_active[failed_disk] = 0;
  rc = disk_array_fail_disk(_da, failed_disk);
  return rc;
}
/**
* Might want this?
//clear out disk with zeroes
int zeroRecover(int new_disk) {
  int rc = 0;
  disk_active[new_disk] = 1;
  rc = disk_array_recover_disk(_da,new_disk);
  return rc;
}*/

