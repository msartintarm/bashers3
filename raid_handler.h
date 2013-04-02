#pragma once
#include "disk-array.h"

void raid_init(disk_array_t the_array, int the_level, int strip_, int disk_);

void raid_disk_array_read(int block_num, int block_size);
void raid_disk_array_write(int block_num, int block_size, int value);
void raid_disk_fail(int disk_num);
void raid_disk_recover(int disk_num);

void printd(char* str);
void printd1(char* str, int arg1);
void printd2(char* string, int arg1, int arg2);
