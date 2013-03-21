#include "raidZero.h"

int strip;
int disk;

int zeroRead(disk_array_t da, int size, int lba) {
  int rc = 0;
  //for size 
  //  call method to translate lba
  //  read, print
  //end
  
  return rc;
}
void setSizes(int s, int d){
  strip = s;
  disk = d;
}

int zeroWrite(disk_array_t da, int size, int lba, char* value) {
  int rc = 0;
  
  int diskIndex  = 0;
  int stripIndex = 0; 
  int blockIndex = 0; //index of block within disk
  int multiple   = 0; //used to move blocks to next set of strips
  int i;
  //loop until address reached
  for(i = 0; i < lba; i++){
    //printf("%4d: [%d,%d,%d]\n", i, diskIndex, stripIndex, blockIndex);
    //if reached end of strip change disk
    if((strip-1) == stripIndex){
      //if reached last disk move back to disk 0
      if(diskIndex == (disk-1)){
        diskIndex = 0;
        multiple += strip;
        stripIndex = 0;
      }
      //not the last disk
      else {
        //blockIndex = multiple;
        diskIndex++;
        stripIndex = 0;
      }
      blockIndex = multiple; //reset to block to strip multiple
      
    }
    //move to next block/strip
    else {
      blockIndex++;
      stripIndex++;
    }
    
  }
  diskIndex = diskIndex % disk;
  //printf("Found: [%d,%d]\n", diskIndex, blockIndex);
  
  return rc;
}


//	      disk_array_recover_disk(da,failed_disk);
//	      disk_array_fail_disk(da,failed_disk);
//	    if (disk_array_read(da, disk, block, buffer) == 0) {
//	    disk_array_write(da, disk, block, buffer);
