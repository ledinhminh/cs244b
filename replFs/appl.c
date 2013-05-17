/************************/
/* Your Name: Wei Shi   */
/* Date: May 9, 2013    */
/* CS 244B              */
/* Spring 2013          */
/************************/

#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <client.h>
#include <appl.h>

#define FS_PORT 40010

static void applme0();
static int appl8();

static int openFile(char *file)
{
    int fd = OpenFile(file);
    if (fd < 0) {
        printf("OpenFile(%s): failed (%d)\n", file, fd);
        exit(-1);
    }
    return fd;
}

static int commit(int fd)
{
    int result = Commit(fd);
    if (result < 0) {
        printf("Commit(%d): failed (%d)\n", fd, result);
        exit(-1);
    }
    return fd;
}

static int closeFile(int fd)
{
    int result = CloseFile(fd);
    if (result < 0) {
        printf("CloseFile(%d): failed (%d)\n", fd, result);
        exit(-1);
    }
    return fd;
}

int main()
{
    if( InitReplFs( FS_PORT, 25, 2) < 0 ) {
        fprintf( stderr, "Error initializing the system\n" );
        return( ErrorExit );
    }
    if(0) applme0();
    if(1) appl8();
    return( NormalExit );
}

static void applme0()
{
    int fd;
    int loopCnt;
    int byteOffset = 0;
    char strData[MaxBlockLength];

    char fileName[32] = "writeTest.txt";

    for ( loopCnt = 0; loopCnt < 8; loopCnt++ ) {
        fd = openFile( fileName );
        sprintf( strData, "%08X", loopCnt );

        if ( WriteBlock( fd, strData, byteOffset, strlen( strData ) ) < 0 ) {
            printf( "Error writing to file %s [LoopCnt=%d]\n", fileName, loopCnt );
        }
        byteOffset += strlen( strData );

        commit( fd );
        closeFile( fd );


    }

}

static int appl8()
{
    int fd;
    int retVal;
    int i;
    char commitStrBuf[512];
    char *filename = (char *)"file8";
    for( i = 0; i < 512; i++ )
        commitStrBuf[i] = '1';

    fd = openFile(filename );

    // write first transaction starting at offset 512
    for (i = 0; i < 50; i++) {
        retVal = WriteBlock( fd, commitStrBuf, 512 + i * 512 , 512 );
    }

    retVal = commit( fd );
    retVal = closeFile( fd );

    for( i = 0; i < 512; i++ )
        commitStrBuf[i] = '2';

    fd = openFile(filename );

    // write second transaction starting at offset 0
    retVal = WriteBlock( fd, commitStrBuf, 0 , 512 );

    retVal = commit( fd );
    retVal = closeFile( fd );


    for( i = 0; i < 512; i++ )
        commitStrBuf[i] = '3';

    fd = openFile(filename);

    // write third transaction starting at offset 50*512
    for (i = 0; i < 100; i++)
        retVal = WriteBlock( fd, commitStrBuf, 50 * 512 + i * 512 , 512 );

    retVal = commit( fd );
    retVal = closeFile( fd );
    return retVal;
}

/* ------------------------------------------------------------------ */
