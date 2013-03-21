CXX=gcc
RM=rm -f
SRCS = disk-array.c disk.c raidZero.c raidsim.c raid_handler.c
OBJS = $(SRCS:.c=.o)
CPPFLAGS = -Wall -Werror -g
LDFLAGS = -Wall -Werror 
#LDLIBS=$(shell root-config --libs)
PROG=raidsim

all: prog

prog: $(OBJS)
	gcc $(LDFLAGS) -o $(PROG) $(OBJS) $(LDLIBS) 

run: prog
	./$(PROG) -level 0 -strip 1 -disks 1 -size 1 -trace sampleTrace.txt -verbose

valgrind: $(OBJS)
	gcc $(LDFLAGS)  -o $(PROG) $(OBJS) $(LDLIBS) 
	valgrind --tool=memcheck --track-origins=yes \
	--leak-check=full  ./$(PROG)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS) $(OBJS2)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
