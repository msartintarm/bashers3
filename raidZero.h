#include "disk-array.h"

/* TODO We need to do some bookeeping right away.
 * Setup the disk structure before hand which will 
 * provide the mapping of the LBA to the proper (disk,block) address
 * Then read/write can just iterate without knowing anything about disk structure
 */ 

/**
 * Read starting at block LBA for SIZE blocks.
 * Then print out the first 4 byte value in each block. 
 */
int zeroRead(disk_array_t da, int size, int lba);

/**
 * Write the 4-byte pattern VALUE repeatedly
 * starting at block LBA for SIZE blocks. 
 */
int zeroWrite(disk_array_t da, int size, int lba, char* value);

int zeroFail(disk_array_t da, int failed_disk);

