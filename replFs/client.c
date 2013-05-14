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
    C = new ClientInstance(FS_PORT, FS_GROUP, packetLoss, numServers);
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
//    fd = open( fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR );
}

/* ------------------------------------------------------------------ */

int
WriteBlock( int fd, char *buffer, int byteOffset, int blockSize )
{
    assert( fd >= 0 );
    assert( byteOffset >= 0 );
    assert( buffer );
    assert( blockSize >= 0 && blockSize < MAX_BLOCK_SIZE );

#ifdef DEBUG
    printf( "WriteBlock: Writing FD=%d, Offset=%d, Length=%d\n",
            fd, byteOffset, blockSize );
#endif
    return C->writeBlock(fd, buffer, byteOffset, blockSize);
    /*
    if ( lseek( fd, byteOffset, SEEK_SET ) < 0 ) {
        perror( "WriteBlock Seek" );
        return(ErrorReturn);
    }

    if ( ( bytesWritten = write( fd, buffer, blockSize ) ) < 0 ) {
        perror( "WriteBlock write" );
        return(ErrorReturn);
    }

    return( bytesWritten );
    */
}

/* ------------------------------------------------------------------ */

int
Commit( int fd )
{
    assert( fd >= 0 );

#ifdef DEBUG
    printf( "Commit: FD=%d\n", fd );
#endif
    return C->commit(fd);

}

/* ------------------------------------------------------------------ */

int
Abort( int fd )
{
    assert( fd >= 0 );

#ifdef DEBUG
    printf( "Abort: FD=%d\n", fd );
#endif

    /*************************/
    /* Abort the transaction */
    /*************************/

    return C->abort(fd);
}

/* ------------------------------------------------------------------ */

int
CloseFile( int fd )
{

    assert( fd >= 0 );

#ifdef DEBUG
    printf( "Close: FD=%d\n", fd );
#endif

    /*****************************/
    /* Check for Commit or Abort */
    /*****************************/
    return C->closeFile(fd);
    /*
    if ( close( fd ) < 0 ) {
        perror("Close");
        return(ErrorReturn);
    }

    return(NormalReturn);
    */
}

/* ------------------------------------------------------------------ */




