#include "string.h"
#include <sys/time.h>
#include <sys/stat.h>
#include <set>

#include "networkInstance.h"

/* Timeouts in ms */
#define OPENFILE_TIMEOUT 1000
#define COMMIT_TIMEOUT 1000

enum serverState{
    Idle, //OpenFile
    Write, //WriteBlock, CommitPrepare, Abort, Close
    CommitReady, //Commit, Close
};

class ServerInstance {
private:
    const std::string mount;
    bool fileOpened;
    int curFd;
    enum serverState state;
    NetworkInstance *N;
    FILE *curFile;

public:
    ServerInstance(unsigned short _port, std::string _mount, int _droprate)
        : mount(_mount), fileOpened(false), curFd(1), state(Idle) {
        N = new NetworkInstance(_port, FS_GROUP, _droprate, true);
        curFile=NULL;
        mkdir(mount.c_str(), S_IRUSR | S_IWUSR);
    };

    ~ServerInstance() {
        delete N;
    }

    void run();

private:
    void handleIdle(PacketBase &p);
    void handleWrite(PacketBase &p);
    void handleCommitReady(PacketBase &p);
};
