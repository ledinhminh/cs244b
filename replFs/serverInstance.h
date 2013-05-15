#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H

#include "string.h"
#include <sys/time.h>
#include <sys/stat.h>
#include <set>
#include <map>
#include <stdio.h>

#include "networkInstance.h"

/* Timeouts in ms */
#define OPENFILE_TIMEOUT 1000
#define COMMIT_TIMEOUT 1000

enum serverState{
    Idle, //OpenFile
    Write, //WriteBlock, CommitPrepare, Abort, Close
    CommitReady, //WriteBlock, Commit, Close
};

class ServerInstance {
private:
    const std::string mount;
    int curFd;
    enum serverState state;
    NetworkInstance *N;
    std::string filepath;
    bool newFile;
    FILE *curFile;
    std::map<uint32_t, PacketWriteBlock> blocks;
    typedef std::map<uint32_t, PacketWriteBlock>::iterator mapit;
    typedef std::vector<uint32_t>::iterator blockIDit;

public:
    ServerInstance(unsigned short _port, std::string _mount, int _droprate)
        : mount(_mount), curFd(1), state(Idle) {
        N = new NetworkInstance(_port, FS_GROUP, _droprate, true);
        curFile=NULL;
        mkdir(mount.c_str(), S_IRUSR | S_IWUSR);
    };

    ~ServerInstance() {
        if(curFile){
            fclose(curFile);
        }
        delete N;
    }

    void run();

private:
    void handleIdle(PacketBase &p);
    void handleWrite(PacketBase &p);
    void handleCommitReady(PacketBase &p);
};

#endif
