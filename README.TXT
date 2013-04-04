Nathan DiPiazza: ndipiazza
Maxwell Kleinhans: mkleinha
Michael Sartin-Tarm: sartin-t

 We ran into 2 minor issues with the code:

 - The new disk.c caused output to be printed differently from '[test.out]' in 
    the 537 results directory, and hence we stuck with the old (pre 4/3) one.

 - Several disks threw uninitialized memory errors when printing,
    and inaccessable ('lost') memory errors when the program finished.

 As the code we wrote does not directly handle disks, we do not think
   these issues arise from our code.



