/************************/
/* Your Name: Wei Shi   */
/* Date: May 9, 2013    */
/* CS 244B              */
/* Spring 2013          */
/************************/

#define FS_PORT 40010

enum {
  NormalReturn = 0,
  ErrorReturn = -1,
};



/* ------------------------------------------------------------------ */

	/********************/
	/* Client Functions */
	/********************/
#ifdef __cplusplus
extern "C" {
#endif

extern int InitReplFs(unsigned short portNum, int packetLoss, int numServers);
extern int OpenFile(char * strFileName);
extern int WriteBlock(int fd, char * strData, int byteOffset, int blockSize);
extern int Commit(int fd);
extern int Abort(int fd);
extern int CloseFile(int fd);

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------ */







