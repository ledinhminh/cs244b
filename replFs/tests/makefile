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
	-rm -f test *.o *.a

run: 
	./runtest --port 40010 --rundir .. --servers 2 2>&1 | tee result

runstress: 
	./runtest       --port 40010 --rundir .. --servers 2 2>&1 | tee resultEasy1
	./runtest -d 20 --port 40010 --rundir .. --servers 5 2>&1 | tee result1
	./runtest       --port 40010 --rundir .. --servers 2 2>&1 | tee resultEasy2
	./runtest -d 20 --port 40010 --rundir .. --servers 5 2>&1 | tee result2
	./runtest -d 20 --port 40010 --rundir .. --servers 5 2>&1 | tee result3
	./runtest -d 20 --port 40010 --rundir .. --servers 5 2>&1 | tee result4
	./runtest -d 20 --port 40010 --rundir .. --servers 5 2>&1 | tee result5
