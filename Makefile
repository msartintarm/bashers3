CXX=gcc
RM=rm -f
SRCS = disk-array.c disk.c test.c
OBJS = $(SRCS:.c=.o)
CPPFLAGS = -Wall -Werror -g
LDFLAGS = -Wall -Werror 
#LDLIBS=$(shell root-config --libs)
PROG=raidsim

all: prog

prog: $(OBJS)
	gcc $(LDFLAGS) -o $(PROG) $(OBJS) $(LDLIBS) 

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
