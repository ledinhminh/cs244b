/************************/
/* Your Name: Wei Shi   */
/* Date: May 9, 2013    */
/* CS 244B              */
/* Spring 2013          */
/************************/

#include <stdio.h>
#include <string.h>

#include <server.h>

/* ------------------------------------------------------------------ */

int main(int argc, char **argv)
{
    if(argc!=NUM_ARGC){
        PRINT("Invalid arguments.\n");
        exit(-1);
    }
    int port=atoi(argv[INDEX_PORT]);
    std::string mount(argv[INDEX_MOUNT]);
    int droprate=atoi(argv[INDEX_DROP]);
    ServerInstance S(port, mount, droprate);
    PRINT("Starting server...\n");
    S.run(); 

    return 0;
}

/* ------------------------------------------------------------------ */
