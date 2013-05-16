#ifndef SERVERINSTANCE_H
#define SERVERINSTANCE_H

#include "string.h"
#include <sys/time.h>
#include <sys/stat.h>
#include <set>
#include <map>
#include <stdio.h>

#include "networkInstance.h"

enum serverState {
    Idle,
    /* OpenFile --[]/OpenFileAck--> Write
     */
    Write,
    /* WriteBlock ----> Write,
     * CommitPrepare --[Has all blocks]/CommitReady--> CommitReady,
     * CommitPrepare --[Missing blocks]/Resend--> Write,
     * Abort ----> Write,
     * Close --[]/CloseAck--> Idle,
     * OpenFile --[Same ID]/OpenfileAck--> Write
     * Commit --[from CommitReady]/CommitSuccess--> Write,
     */
    CommitReady, 
    /* WriteBlock ----> Write,
     * Commit --[]/CommitSuccess--> Write,
     * CommitPrepare --[]/CommitReady--> CommitReady,
     * Close --[]/CloseAck--> Idle
     */
};

class ServerInstance {
private:
    const std::string mount;
    int curFd;
    int closedFd;
    enum serverState state;
    enum serverState lastState;
    NetworkInstance *N;
    std::string filepath;
    bool newFile;
    FILE *curFile;
    std::map<uint32_t, PacketWriteBlock> blocks;
    typedef std::map<uint32_t, PacketWriteBlock>::iterator mapit;
    typedef std::set<uint32_t>::iterator blockIDit;

public:
    ServerInstance(unsigned short _port, std::string _mount, int _droprate)
        : mount(_mount), curFd(1), state(Idle), lastState(Idle){
        N = new NetworkInstance(_port, FS_GROUP, _droprate, true);
        curFile = NULL;
        if(mkdir(mount.c_str(), S_IRUSR | S_IWUSR)<0){
            PRINT("MKDIR errno = %s\n", strerror(errno));
            throw FSException("Cannot create dir");
        };
    };

    ~ServerInstance() {
        if(curFile) {
            fclose(curFile);
        }
        delete N;
    }

    void run();

private:
    void transit(enum serverState next);
    void handleIdle(PacketBase &p);
    void handleWrite(PacketBase &p);
    void handleCommitReady(PacketBase &p);
};

#endif
