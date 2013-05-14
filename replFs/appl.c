/************************/
/* Your Name: Wei Shi   */
/* Date: May 9, 2013    */
/* CS 244B              */
/* Spring 2013          */
/************************/

#define DEBUG

#include <stdio.h>
#include <string.h>

#include <client.h>
#include <appl.h>

/* ------------------------------------------------------------------ */

int
main()
{

    int fd;
    int loopCnt;
    int byteOffset = 0;
    char strData[MaxBlockLength];

    char fileName[32] = "writeTest.txt";

    /*****************************/
    /* Initialize the system     */
    /*****************************/

    if( InitReplFs( FS_PORT, 0, 1 ) < 0 ) {
        fprintf( stderr, "Error initializing the system\n" );
        return( ErrorExit );
    }

    /*****************************/
    /* Open the file for writing */
    /*****************************/

    fd = OpenFile( fileName );
    if ( fd < 0 ) {
        fprintf( stderr, "Error opening file '%s'\n", fileName );
        return( ErrorExit );
    } else {
        fprintf( stderr, "OK: Opening file fd=%d\n", fd );
    }

    /**************************************/
    /* Write incrementing numbers to the file */
    /**************************************/

    for ( loopCnt = 0; loopCnt < 4; loopCnt++ ) {
        sprintf( strData, "%d\n", loopCnt );

#ifdef DEBUG
        printf( "%d: Writing '%s' to file.\n", loopCnt, strData );
#endif

        if ( WriteBlock( fd, strData, byteOffset, strlen( strData ) ) < 0 ) {
            printf( "Error writing to file %s [LoopCnt=%d]\n", fileName, loopCnt );
            return( ErrorExit );
        }
        byteOffset += strlen( strData );

    }


    return( NormalExit );


    /**********************************************/
    /* Can we commit the writes to the server(s)? */
    /**********************************************/
    if ( Commit( fd ) < 0 ) {
        printf( "Could not commit changes to File '%s'\n", fileName );
        return( ErrorExit );
    }

    /**************************************/
    /* Close the writes to the server(s) */
    /**************************************/
    if ( CloseFile( fd ) < 0 ) {
        printf( "Error Closing File '%s'\n", fileName );
        return( ErrorExit );
    }

    printf( "Writes to file '%s' complete.\n", fileName );

    return( NormalExit );
}

/* ------------------------------------------------------------------ */
