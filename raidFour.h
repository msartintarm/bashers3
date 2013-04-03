#include "disk-array.h"

/**
 * Initialize things RAID needs for every call.
 */
void fourInit(disk_array_t da, int num_disks,
	      int strip_size, int disk_size);
/**
 * Read starting at block LBA for SIZE blocks.
 * Then print out the first 4 byte value in each block. 
 */
int fourRead(int size, int lba);

/**
 * Write the 4-byte pattern VALUE repeatedly
 * starting at block LBA for SIZE blocks. 
 */
int fourWrite(int size, int lba, char* value);

int fourFail(int failed_disk);
int fourRecover(int recovered_disk);
