#include <stdlib.h>
#include <stdio.h>

// disk 0 gets blocks 0-1 and 6-7,
// disk 1 gets blocks 2-3 and 8-9, &
// disk 2 gets blocks 4-5 and 10-11.

void fakePrint(int d, int b){
  printf("Writing [disk,block]: [%d,%d]\n", d, b);
}

int verbose = 0;

int main( int argc, char *argv[] ) {
  
  int strip = atoi(argv[1]);
  int disk =  atoi(argv[2]);
  int lba =   atoi(argv[3]);
  int size =  atoi(argv[4]);
  short startFound = 0;
  int diskIndex = 0;
  int stripIndex = 0;
  int i;
  int multiple = 0;
  int blockIndex = 0;
  //loop until address reached
  for(i = 0; i < size + lba; i++){
    if(i == lba){
      startFound = 1;
    }
    if(startFound == 1){
      fakePrint(diskIndex, blockIndex);
    }
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
    else {
      blockIndex++;
      stripIndex++;
    }
    
  }
  diskIndex = diskIndex % disk;
  
  //printf("[disk,block]: [%d,%d]\n", diskIndex, blockIndex);
  return 0;
}
