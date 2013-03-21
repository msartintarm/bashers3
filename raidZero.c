#include "raidZero.h"

int zeroRead(disk_array_t da, int size, int lba) {
  int rc = 0;
  //for size 
  //  call method to translate lba
  //  read, print
  //end
  
  return rc;
}

int zeroWrite(disk_array_t da, int size, int lba, char* value) {
  int rc = 0;
  //call method to translate lba
  //for i = 0; i < size; i++
  //  write value to lba++
  //end
  
  return rc;
}


//	      disk_array_recover_disk(da,failed_disk);
//	      disk_array_fail_disk(da,failed_disk);
//	    if (disk_array_read(da, disk, block, buffer) == 0) {
//	    disk_array_write(da, disk, block, buffer);
