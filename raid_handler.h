#pragma once
#include "disk-array.h"

void raid_init(int the_level, 
			         int strip_size, int num_disks, int disk_size);
/**
 * call functions to free up memory.
 */		
void raid_cleanup();
/**
 * handler chooses the approriate
 * raid level to perform a read operation.
 */
void raid_disk_array_read(int block_num, int block_size);
/**
 * handler chooses the approriate
 * raid level to perform a write operation.
 */
void raid_disk_array_write(int block_num, int block_size, int value);
/**
 * mark a disk as failed
 */
void raid_disk_fail(int disk_num);
/**
 * handler chooses the approriate
 * level to recover a disk
 */
void raid_disk_recover(int disk_num);
/**
 * Custom verbose print functions used for testing
 */
void printd(char* str);
void printd1(char* str, int arg1);
void printd2(char* string, int arg1, int arg2);
