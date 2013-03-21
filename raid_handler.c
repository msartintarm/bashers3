#include "raid_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void raid_disk_array_read(int block_num, int block_size) {
  printf("Read block number %d with size %d.\n", block_num, block_size);

}

void raid_disk_array_write(int block_num, int block_size, char* value) {
  
  printf("Read block number %d with size %d.\n", block_num, block_size);
}

void raid_disk_fail(int disk_num) {

 printf("Disk %d failure.\n", disk_num);
}

void raid_disk_recover(int disk_num) {

  printf("Disk %d recovery.\n", disk_num);
}


