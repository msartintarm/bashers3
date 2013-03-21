#pragma once




void raid_disk_array_read(int block_num, int block_size);
void raid_disk_array_write(int block_num, int block_size, char* value);
void raid_disk_fail(int disk_num);
void raid_disk_recover(int disk_num);

