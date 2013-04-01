#include "disk-array.h"

/**
 * Read starting at block LBA for SIZE blocks.
 * Then print out the first 4 byte value in each block. 
 */
int fourRead(disk_array_t da, int size, int lba);

/**
 * Write the 4-byte pattern VALUE repeatedly
 * starting at block LBA for SIZE blocks. 
 */
int fourWrite(disk_array_t da, int size, int lba, char* value);

int fourFail(disk_array_t da, int failed_disk);

