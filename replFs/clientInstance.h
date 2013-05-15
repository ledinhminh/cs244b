#include "string.h"
#include "sys/time.h"
#include <set>
#include <map>

#include "networkInstance.h"

/* Timeouts in ms */
#define OPENFILE_TIMEOUT 1000
#define COMMITPREPARE_TIMEOUT 1000
#define COMMIT_TIMEOUT 1000

class ClientInstance {
private:
    NetworkInstance *N;
    unsigned int numServers;
    std::set<uint32_t> servers;
    std::map<uint32_t, PacketWriteBlock> blocks;
    typedef std::map<uint32_t, PacketWriteBlock>::iterator mapit;
    typedef std::vector<uint32_t>::iterator blockIDit;
    bool fileOpened;
    int curFd;
    int curBlockID;
    timeval timeOpenFile;
    timeval timeCommitPrepare;
    timeval timeCommit;

public:
    ClientInstance(unsigned short _port, int _droprate, int _numServers) {
        numServers = _numServers;
        fileOpened = false;
        curFd = 1;
        curBlockID = 1;
        N = new NetworkInstance(_port, FS_GROUP, _droprate, false);
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
    int commitFinal();
    void setCurrentTime(timeval *t);
    bool isTimeout(timeval startup, long timeout);
    long timediff(timeval t1, timeval t2);
};
