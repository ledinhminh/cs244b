CFLAGS	= -g -Wall -DSUN
# CFLAGS	= -g -Wall -DDEC
CC	= g++
CCF	= $(CC) $(CFLAGS)

H	= .
C_DIR	= .

INCDIR	= -I$(H)
LIBDIRS = -L$(C_DIR)
LIBS    = -lclientReplFs

CLIENT_OBJECTS = network.o clientInstance.o client.o

all:	appl

appl:	appl.o $(C_DIR)/libclientReplFs.a
	$(CCF) -o appl appl.o $(LIBDIRS) $(LIBS)

appl.o:	appl.c client.h appl.h
	$(CCF) -c $(INCDIR) appl.c

$(C_DIR)/libclientReplFs.a:	$(CLIENT_OBJECTS)
	ar cr libclientReplFs.a $(CLIENT_OBJECTS)
	ranlib libclientReplFs.a

client.o:	client.c client.h network.cpp network.h packet.h
	$(CCF) -c $(INCDIR) client.c

clientInstance.o: clientInstance.cpp clientInstance.h
	$(CCF) -c $(INCDIR) clientInstance.cpp

network.o: network.h network.cpp exception.h packet.h
	$(CCF) -c $(INCDIR) network.cpp

#%.o: %.cpp %.h exception.h
#	$(CCF) -c $(INCDIR) $< -o $@

clean:
	rm -f appl *.o *.a

