#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "raidFour.h"

int verbose = 0;

int main( int argc, char *argv[] ) {

  int strip = atoi(argv[1]);
  int disks =  atoi(argv[2]);
  int lba =   atoi(argv[3]);
  int size =  atoi(argv[4]);
  
  disk_array_t da = disk_array_create("myvirtualdisk", disks, 30);
  if(!da) {
    fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
    return 1;
  }
  fourInit(da, disks, strip);

  fourWrite(size, lba, "error");



  return 0;
}
