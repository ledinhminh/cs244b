#include "network.h"
#include "string.h"
#include "sys/time.h"

/* Timeouts in ms */
#define OPENFILE_TIMEOUT 1000
#define COMMIT_TIMEOUT 1000

class ClientInstance {
private:
    NetworkInstance *N;
    bool fileOpened;
    int curFd;
    timeval timeOpenFile;
    timeval timeCommit;

public:
    ClientInstance(unsigned short _port, unsigned int _group, int _droprate) {
        fileOpened = false;
        curFd = 0;
        N = new NetworkInstance(_port, _group, _droprate);
    };

    ~ClientInstance() {
        delete N;
    }
    int openFile(char *strFileName);
    int writeBlock(int fd, char *strData, int byteOffset, int blockSize);
    int commit(int fd);
    int abort(int fd);
    int closeFile(int fd);

private:
    void setCurrentTime(timeval *t);
    bool isTimeout(timeval startup, long timeout);
    long timediff(timeval t1, timeval t2);
};
