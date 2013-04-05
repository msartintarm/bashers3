#include "raidTen.h"
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
int num_disks;
int disk_size;
char buffer[BLOCK_SIZE];
static disk_array_t _da;
static int* disk_active; // 0 denotes the disk is failed

/**
 * initialize needed vars & malloc array 
 */
void tenInit(disk_array_t da, int strip, int disk, int disk_size_) {
  _da = da;
  _strip = strip;
  num_disks = disk;
  disk_size = disk_size_;
  disk_active = malloc(num_disks * sizeof(int));  //malloc array to keep track of failed disks
  int i;
  // All disks start out as active (not failed)
  for(i = 0; i < num_disks; ++i) {
    disk_active[i] = 1;
  }
}

void tenCleanup() { free(disk_active); } //deconstructor

/*RAID 10
*
 Blocks should be striped across disks, starting with the lowest numbered disk and increasing.
 Each disk gets a strip of blocks, which is one or more   blocks. The i+1 disk is i's mirror.
 For example, with 6 disks and strip size of 2,
 disk 0 gets blocks 0-1 and 6-7,
 disk 1 is its mirror
 disk 2 gets blocks 2-3 and 8-9,
 disk 3 is its mirror,
 disk 4 gets blocks 4-5 and 10-11, and
 disk 5 is its mirror.
*/

/**
 * This function handles striping across the disks
 */
static int stripper(int size, int lba, char* value, short isWrite) {
  int rc = 0; //return code var
  short startFound = 0; //reached lba
  int diskIndex  = 0; //index of disk in disk_array
  int blockIndex = 0; //index of block within disk
  int i;

  int writeError = 0; //boolean to determine to print write-error
  //loop until address reached
  for(i = 0; i < size + lba; i++){
    if(i == lba){
      startFound = 1;
    }
    //if address reached perform read or write
    if(startFound == 1){
      if(isWrite == 1){
        //write to disk and its mirror
		    disk_array_write(_da, diskIndex, blockIndex, value);
		    disk_array_write(_da, diskIndex+1, blockIndex, value);
		    //if both disk and mirror failed set bool
		    if(!disk_active[diskIndex] && !disk_active[diskIndex+1]) {
		      writeError = 1;
		    }
      }
      //Read operation
      else {
        disk_array_read(_da, diskIndex, blockIndex, buffer);
        if(disk_active[diskIndex]) {
	      printf("%d ", *((int*)buffer));
        //If that disk is failed try its mirror
		    } 
		    //disk failed
		    else {
          disk_array_read(_da, diskIndex+1, blockIndex, buffer);
          //is mirror bad too?
          if(disk_active[diskIndex+1]) {
              printf("%d ", *((int*)buffer));
          }
          //print both bad 
          else { printf("ERROR "); }
		    }
      }
    } //end if start found
    
  //if reached end of strip change disk
	if(++blockIndex % _strip == 0) {
	  diskIndex += 2;
	  if(diskIndex % num_disks == 0) {
		diskIndex = 0;
	  } else {
		blockIndex -= _strip;
	  }
	}
  }
  //inform user of write error
  if(writeError) { printf("ERROR "); }
  diskIndex = diskIndex % num_disks;
  return rc; //bubble up error message
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
  disk_active[failed_disk] = 0; //clear active bit
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
  disk_active[new_disk] = 1; //set disk index of array to active=1

  // Odd number is 'normal' disk, even number is mirror
  int copy_disk = (new_disk % 2 == 0)? new_disk + 1: new_disk - 1;

  rc = disk_array_recover_disk(_da,new_disk);

  int i;
  for(i = 0; i < disk_size; i++) {
	disk_array_read(_da, copy_disk, i, buffer); 
	disk_array_write(_da, new_disk, i, buffer);
  }

  return rc;
}

