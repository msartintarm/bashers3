CXX=gcc
RM=rm -f
SRCS = disk-array.c disk.c raidZero.c raidFour.c raidTen.c raid_handler.c
OBJS = $(SRCS:.c=.o)
CPPFLAGS = -Wall -Werror -g
LDFLAGS = -Wall -Werror 
#LDLIBS=$(shell root-config --libs)
PROG=raidsim
RAID0=zeroTest
RAID4=fourTest

all: prog

prog: $(OBJS) $(PROG).o
	gcc $(LDFLAGS) -o $(PROG) $(OBJS) $(PROG).o $(LDLIBS) 

raid0: $(OBJS) $(RAID0).o
	gcc $(LDFLAGS) -o $(RAID0) $(OBJS) $(RAID0).o $(LDLIBS) 
	./$(RAID0) 2 2 0 2
	./$(RAID0) 2 2 1 2
	./$(RAID0) 2 2 2 2
	./$(RAID0) 1 2 3 2

raid4: $(OBJS) $(RAID4).o
	gcc $(LDFLAGS) -o $(RAID4) $(OBJS) $(RAID4).o $(LDLIBS) 
	./$(RAID4) 2 3 0 2
	./$(RAID4) 2 3 1 2
	./$(RAID4) 2 3 2 2
	./$(RAID4) 1 3 3 2

run: prog
	./$(PROG) -level 0 -strip 1 -disks 1 -size 1 -trace sampleTrace.txt -verbose

valgrind: $(OBJS)
	gcc $(LDFLAGS)  -o $(PROG) $(OBJS) $(LDLIBS) 
	valgrind --tool=memcheck --track-origins=yes \
	--leak-check=full  ./$(PROG)

depend: .depend

.depend: $(SRCS) $(PROG).c $(RAID0).c $(RAID4).c
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
