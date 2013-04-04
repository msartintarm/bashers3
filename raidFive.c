#include "raidFive.h"
#include "raid_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*RAID 5
*
*/

// -- Local variables -- //

static int i;
static disk_array_t disk_arr;
static int num_disks;
#define num_disks
static int strip_size;
static int disk_size;
static char buffer[BLOCK_SIZE];
static int parityBuff[BLOCK_SIZE];
static int* disk_active; // 0 denotes the disk is failed

// -- Local functions ('methods') -- //

/**
  Updates parity for a single block 
  using the subtractive technique.
 */
static void subtractiveParity(int disk_num, int block_offset, char* oldData,
				 char* newData, int parity_disk);

/**
  Updates parity block using the additive technique.

  The disk number is the one for which the new data is passed. 
  It's never accessed or read (all others are).

  This is because we use it in the case where the disk is invalid.
 */
static void additiveParity(int disk_num, int block_offset, 
				char* newData, int parity_disk);
/**
   If a disk is restored, its contents will be zeroed out and
   hence, we must redetermine the data using additive parity.
*/
static void restoredParity(int restored_disk);

void fiveInit(disk_array_t da, int strip_size_,
	      int num_disks_, int disk_size_) {
  disk_arr = da;
  num_disks = num_disks_;
  strip_size = strip_size_;
  disk_size = disk_size_;
  disk_active = malloc(num_disks * sizeof(int));
  // All disks start out as active (not failed)
  for(i = 0; i < num_disks; ++i) {
    disk_active[i] = 1;
  }
}

void fiveCleanup() { free(disk_active); }

/**
 * yeah I went there. Strip those disks!
 */
static int stripper(int size, int lba, char* value, short isWrite) {

  // Find starting disk number and offset.
  int block_offset = lba % strip_size;
  int strip = lba / strip_size; 
  int disk_num = strip % (num_disks - 1); 
  int stripe = strip / (num_disks - 1);
  //disk on which parity bits are stored
  int parity_disk = stripe % num_disks; 
  if(disk_num >= parity_disk) disk_num++;
  block_offset += stripe * strip_size;

  printd1(" Block address: %d\n", lba);
  printd2(" Starting block offset: %d, disk Num: %d\n", block_offset, disk_num);


  for(i = 0; i < size; ++i) {
   
    if(isWrite == 1){ // Write operation
      if(!disk_active[disk_num]) {
	printd1("Can't write to failed disk %d\n", disk_num);
	additiveParity(disk_num, block_offset, value, parity_disk);
      } else {
	// Read old data, update parity, and write new data
	disk_array_read(disk_arr, disk_num, block_offset, buffer);
	subtractiveParity(disk_num, block_offset, buffer,
			 value, parity_disk);
	disk_array_write(disk_arr, disk_num, block_offset, value);
      }

    } else { //Read operation
      if(disk_active[disk_num]) {
	disk_array_read(disk_arr, disk_num, block_offset, buffer);
	printf("%d\n", *((int*)buffer));
      } else {
	printd2("Reconstructing disk %d index %d from parity.\n", disk_num, block_offset);
	disk_array_read(disk_arr, parity_disk, block_offset, (char*)parityBuff);
	int j;
	for(j = 0; j < num_disks; ++j) {
	  if(!disk_active[j] || j == parity_disk) continue;
	    disk_array_read(disk_arr, j, block_offset, buffer);
	    parityBuff[0] ^= *(int*)buffer;
	}
	printd2("Reconstruction result: *", disk_num, block_offset);
	printf("%d", *((int*)parityBuff));
	printd("*");
	printf("\n");
      }
    }
    // Compute new index
    if(++block_offset % strip_size == 0) {
	  // Iterate disk!
	  if(++disk_num % num_disks == 0) {
		// New set of indexes!
		stripe = block_offset / strip_size; 
    		parity_disk = stripe % num_disks;
		disk_num = 0;
	  } else {
		// Otherwise, keep same index set
		block_offset -= strip_size;
	  }
	  if(disk_num == parity_disk) disk_num++;
    }
  }
  return 0;
}

int fiveRead(int size, int lba) {
  return stripper(size, lba, NULL, 0);
}

int fiveWrite(int size, int lba, char* value) {
  return stripper(size, lba, value, 1);
}

int fiveFail(int failed_disk) {
  disk_active[failed_disk] = 0;
  return disk_array_fail_disk(disk_arr, failed_disk);
}

int fiveRecover(int recovered_disk) {

  int rv = disk_array_recover_disk(disk_arr, recovered_disk);
  disk_active[recovered_disk] = 1;
  restoredParity(recovered_disk);
  return rv;
}

static void subtractiveParity(int disk_num, int block_offset, char* oldData,
				 char* newData, int parity_disk) {
  // Obtain the old parity
  disk_array_read(disk_arr, parity_disk, block_offset, (char*)parityBuff);
  printd2(" Parity bit for index %d: from %d ", block_offset, *parityBuff);

  parityBuff[0] ^= *(int*)oldData; // Remove old data from parity
  parityBuff[0] ^= *(int*)newData; // Add new data to parity

  printd1("to %d.\n", parityBuff[0]);

  disk_array_write(disk_arr, parity_disk, block_offset, (char*)parityBuff);
}

static void additiveParity(int disk_num, int block_offset,
				 char* newData, int parity_disk) {
  int ii;
  // Don't need to read parity disk - instead, set to 0.
  *parityBuff = 0;
  for(ii = 0; ii < num_disks; ++ii) {
	if(ii == disk_num || ii == parity_disk) continue; // This is always 0.
	disk_array_read(disk_arr, ii, block_offset, buffer);
	*parityBuff ^= *(int*)buffer;
  }

  // Finally, XOR the new data (that we would write to the disk if we could).
  *parityBuff ^= *(int*)newData;
  
  printd2("Disk %d index %d has failed, so we updated parity.\n", disk_num, block_offset);
  disk_array_write(disk_arr, parity_disk, block_offset, (char*)parityBuff);
}

static void restoredParity(int restored_disk) {
  int k, ii;
  for(k = 0; k < disk_size; ++k){
    int stripe = k % (disk_size / strip_size);
    int parity_disk = stripe % num_disks;
    disk_array_read(disk_arr, parity_disk, k, (char*)parityBuff);
    for(ii = 0; ii < num_disks; ++ii) {
      if(ii == restored_disk) continue; // This is always 0.
      disk_array_read(disk_arr, ii, k, buffer);
      *parityBuff ^= *(int*)buffer;
    }
    // Parity XOR'ed with everything other than the data.. yields the data.
    printd2("Disk %d index %d", restored_disk, k);
    printd1(" recovered with data %d.\n", parityBuff[0]);
    disk_array_write(disk_arr, restored_disk, k, (char*)parityBuff);
  }
}

