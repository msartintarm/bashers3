#include "disk-array.h"

/**
 * Initialize vars need by raid.
 */
void tenInit(disk_array_t da, int strip, int disk, int asdas);
/**
 * Read starting at block LBA for SIZE blocks.
 * Then print out the first 4 byte value in each block. 
 */
int tenRead(int size, int lba);
/**
 * Write the 4-byte pattern VALUE repeatedly
 * starting at block LBA for SIZE blocks. 
 */
int tenWrite(int size, int lba, char* value);
/**
 * Mark a disk as failed
 */
int tenFail(int failed_disk);
/**
 * Recover a disk by clearing disk & 
 * copying its mirror over to new disk
 */
int tenRecover(int new_disk);
/**
 * free up memory
 */
void tenCleanup();

