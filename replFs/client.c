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
    C=new ClientInstance(FS_PORT, FS_GROUP, packetLoss);
    std::stringstream sink;
    PacketCommit cp;
    cp.id = 1;
    cp.seqNum = 42;
    cp.fileID = 37;
    cp.serialize(sink);

    for (std::string::size_type i = 0; i < sink.str().length(); ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)sink.str()[i];
    }
    std::cout << std::endl;


    /****************************************************/
    /* Initialize network access, local state, etc.     */
    /****************************************************/
    return -1;
    return( NormalReturn );
}

/* ------------------------------------------------------------------ */

int
OpenFile( char *fileName )
{
    int fd;

    assert( fileName );

#ifdef DEBUG
    printf( "OpenFile: Opening File '%s'\n", fileName );
#endif

    fd = open( fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR );

#ifdef DEBUG
    if ( fd < 0 )
        perror( "OpenFile" );
#endif

    return( fd );
}

/* ------------------------------------------------------------------ */

int
WriteBlock( int fd, char *buffer, int byteOffset, int blockSize )
{
    //char strError[64];
    int bytesWritten;

    assert( fd >= 0 );
    assert( byteOffset >= 0 );
    assert( buffer );
    assert( blockSize >= 0 && blockSize < MAX_BLOCK_SIZE );

#ifdef DEBUG
    printf( "WriteBlock: Writing FD=%d, Offset=%d, Length=%d\n",
            fd, byteOffset, blockSize );
#endif

    if ( lseek( fd, byteOffset, SEEK_SET ) < 0 ) {
        perror( "WriteBlock Seek" );
        return(ErrorReturn);
    }

    if ( ( bytesWritten = write( fd, buffer, blockSize ) ) < 0 ) {
        perror( "WriteBlock write" );
        return(ErrorReturn);
    }

    return( bytesWritten );

}

/* ------------------------------------------------------------------ */

int
Commit( int fd )
{
    assert( fd >= 0 );

#ifdef DEBUG
    printf( "Commit: FD=%d\n", fd );
#endif

    /****************************************************/
    /* Prepare to Commit Phase			    */
    /* - Check that all writes made it to the server(s) */
    /****************************************************/

    /****************/
    /* Commit Phase */
    /****************/

    return( NormalReturn );

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

    return(NormalReturn);
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

    if ( close( fd ) < 0 ) {
        perror("Close");
        return(ErrorReturn);
    }

    return(NormalReturn);
}

/* ------------------------------------------------------------------ */




