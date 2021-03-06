#include "raidZero.h"
#include "raid_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 
 * Raid implementation of RAID 0
 * strip data across disks
 * 
 */ 

int _strip;
int _disk;
char buffer[BLOCK_SIZE];
static disk_array_t _da;
static int* disk_active; // 0 denotes the disk is failed

/**
 * initialize needed vars & malloc array 
 */
void zeroInit(disk_array_t da, int strip, int disk) {
  _da = da;
  _strip = strip;
  _disk = disk;
  int i;
  disk_active = malloc(_disk * sizeof(int)); //malloc array to keep track of failed disks
  // All disks start out as active (not failed)
  for(i = 0; i < _disk; ++i) {
    disk_active[i] = 1;
  }
}

void zeroCleanup() { free(disk_active); } //deconstructor

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
 * This function handles striping across the disks
 */
static int stripper(int size, int lba, char* value, short isWrite) {
  int rc = 0;
  short startFound = 0;
  int diskIndex  = 0;
  int blockIndex = 0; //index of block within disk
  int i;
  short write_error = 0;
  //loop until address reached
  for(i = 0; i < size + lba; i++){
    if(i == lba){
      startFound = 1;
    }
    //if address reached perform read or write
    if(startFound == 1){
      if(isWrite == 1){
        //has the disk failed?
        if(disk_active[diskIndex] == 0) {
          write_error = 1;
        }
		    disk_array_write(_da, diskIndex, blockIndex, value);
      }
      //Read operation
      else {
		    disk_array_read(_da, diskIndex, blockIndex, buffer);
		    //if bad disk print error message
        if(!disk_active[diskIndex]) {
          printf("ERROR ");
        }
        else {
	        printf("%d ", *((int*)buffer)); //print output from read
        }
      }
    } //end if start found
    
    //if reached end of strip change disk
	  if(++blockIndex % _strip == 0) {
	    diskIndex += 1;
	    if(diskIndex % _disk == 0) {
		    diskIndex = 0;
	    }
	    else {
		    blockIndex -= _strip;
	    }
	  }
	
  } //end size+lba for loop
  diskIndex = diskIndex % _disk;
  //inform user of write error
  if(write_error){
    printf("ERROR ");
    write_error = 0;
  }
  
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

//clear out disk with zeroes
int zeroRecover(int new_disk) {
  int rc = 0;
  disk_active[new_disk] = -1; //special case for write handling
  rc = disk_array_recover_disk(_da,new_disk);
  return rc;
}

