#include "disk-array.h"

/**
 * Initialize vars need by raid.
 */
void zeroInit(disk_array_t da, int strip, int disk);

/**
 * Read starting at block LBA for SIZE blocks.
 * Then print out the first 4 byte value in each block. 
 */
int zeroRead(int size, int lba);
/**
 * Write the 4-byte pattern VALUE repeatedly
 * starting at block LBA for SIZE blocks. 
 */
int zeroWrite(int size, int lba, char* value);
/**
 * Mark a disk as failed
 */
int zeroFail(int failed_disk);
/**
 * not much can be done except to zero out the disk
 */
int zeroRecover(int failed_disk);
/**
 * free up memory
 */
void zeroCleanup();
