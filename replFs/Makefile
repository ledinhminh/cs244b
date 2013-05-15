CFLAGS	= -g -Wall -DSUN 
# CFLAGS	= -g -Wall -DDEC
CC	= g++
CCF	= $(CC) $(CFLAGS)

H	= .
C_DIR	= .

INCDIR	= -I$(H)
LIBDIRS = -L$(C_DIR)
LIBS    = -lclientReplFs
includes = $(wildcard *.h)

CLIENT_OBJECTS = packet.o networkInstance.o clientInstance.o client.o
SERVER_OBJECTS = packet.o networkInstance.o serverInstance.o server.o

all:	appl replFsServer

replFsServer: $(SERVER_OBJECTS)
	$(CCF) $(SERVER_OBJECTS) -o $@

appl:	appl.o $(C_DIR)/libclientReplFs.a
	$(CCF) -o appl appl.o $(LIBDIRS) $(LIBS)

appl.o:	appl.c client.h appl.h
	$(CCF) -c $(INCDIR) appl.c

$(C_DIR)/libclientReplFs.a:	$(CLIENT_OBJECTS)
	ar cr libclientReplFs.a $(CLIENT_OBJECTS)
	ranlib libclientReplFs.a

client.o:	client.c client.h $(includes)
	$(CCF) -c $(INCDIR) client.c

%.o: %.cpp %.h $(includes)
	$(CCF) -c $(INCDIR) $< -o $@

clean:
	rm -f appl replFsServer *.o *.a
	rm -r dir


s:
	./replFsServer -port 40010 -mount dir -drop 0

c:
	./appl

