/************************/
/* Your Name: Wei Shi   */
/* Date: May 9, 2013    */
/* CS 244B              */
/* Spring 2013          */
/************************/

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

int InitReplFs( unsigned short portNum, int packetLoss, int numServers )
{
    PRINT( "InitReplFs: Port number %d, packet loss %d percent, %d servers\n",
           portNum, packetLoss, numServers );
    C = new ClientInstance(portNum, packetLoss, numServers);
    return( NormalReturn );
}

int OpenFile( char *fileName )
{
    PRINT( "OpenFile: Opening File '%s'\n", fileName );
    return C->openFile(fileName);
}

int WriteBlock( int fd, char *buffer, int byteOffset, int blockSize )
{
    return C->writeBlock(fd, buffer, byteOffset, blockSize);
}

int Commit( int fd )
{
    PRINT( "Commit: FD=%d\n", fd );
    return C->commit(fd);

}

int Abort( int fd )
{
    PRINT( "Abort: FD=%d\n", fd );
    return C->abort(fd);
}

int CloseFile( int fd )
{
    PRINT( "Close: FD=%d\n", fd );
    return C->closeFile(fd);
}


