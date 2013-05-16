/************************/
/* Your Name: Wei Shi   */
/* Date: May 9, 2013    */
/* CS 244B              */
/* Spring 2013          */
/************************/

#define DEBUG

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <iomanip>

#include "client.h"
#include "clientInstance.h"

static ClientInstance *C;

/* ------------------------------------------------------------------ */

int
InitReplFs( unsigned short portNum, int packetLoss, int numServers )
{
#ifdef DEBUG
    printf( "InitReplFs: Port number %d, packet loss %d percent, %d servers\n",
            portNum, packetLoss, numServers );
#endif
    C = new ClientInstance(FS_PORT, packetLoss, numServers);
    return( NormalReturn );
}

/* ------------------------------------------------------------------ */

int
OpenFile( char *fileName )
{
    assert( fileName );

#ifdef DEBUG
    printf( "OpenFile: Opening File '%s'\n", fileName );
#endif

    return C->openFile(fileName);
}

/* ------------------------------------------------------------------ */

int
WriteBlock( int fd, char *buffer, int byteOffset, int blockSize )
{
#ifndef DEBUG
    printf( "WriteBlock: Writing FD=%d, Offset=%d, Length=%d\n",
            fd, byteOffset, blockSize );
#endif
    return C->writeBlock(fd, buffer, byteOffset, blockSize);
}

/* ------------------------------------------------------------------ */

int
Commit( int fd )
{
#ifdef DEBUG
    printf( "Commit: FD=%d\n", fd );
#endif
    return C->commit(fd);

}

/* ------------------------------------------------------------------ */

int
Abort( int fd )
{
#ifdef DEBUG
    printf( "Abort: FD=%d\n", fd );
#endif

    return C->abort(fd);
}

/* ------------------------------------------------------------------ */

int
CloseFile( int fd )
{

#ifdef DEBUG
    printf( "Close: FD=%d\n", fd );
#endif

    return C->closeFile(fd);
}


