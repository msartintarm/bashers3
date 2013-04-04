#include "disk-array.h"

/**
   Initialize things RAID needs for every call.
 */
void fiveInit(disk_array_t da, int num_disks,
	      int strip_size, int disk_size);
/**
   A little garbage collection for the boys.
*/
void fiveCleanup();

/**
   Read starting at block LBA for SIZE blocks.
   Then print out the first 4 byte value in each block. 
 */
int fiveRead(int size, int lba);

/**
   Write the 4-byte pattern VALUE repeatedly
   starting at block LBA for SIZE blocks. 
 */
int fiveWrite(int size, int lba, char* value);

int fiveFail(int failed_disk);
int fiveRecover(int recovered_disk);
