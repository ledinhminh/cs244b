RUNDIR = ..
ifndef RUNDIR
    $(error You must specify RUNDIR)
endif
CFLAGS = -g -DSUN -lssl -fpermissive
CC = g++
CCF = $(CC) $(CFLAGS)

all: test

test: test.o $(RUNDIR)/libclientReplFs.a
	$(CCF) -o test test.o -L$(RUNDIR) -lclientReplFs

test.o: test.c
	$(CCF) -I$(RUNDIR) -c test.c

clean:
	rm -f test *.o *.a

run1:
	rm result
	./runtest --port 40010 --rundir .. --servers 2 2>&1 | tee result

run:
	-rm resul*
	./runtest -d 20 --port 40010 --rundir .. --servers 5 2>&1 | tee result
#	./runtest -d 30 --port 40010 --rundir .. --servers 8 2>&1 | tee resultExtra
