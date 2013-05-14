#include "clientInstance.h"

int ClientInstance::openFile(char *strFileName)
{
    if(fileOpened || strlen(strFileName) >= MAX_FILENAME_SIZE) {
        return -1;
    }
    /* Send request */
    PacketOpenFile p;
    strcpy(p.filename, strFileName);
    N->send(p);
    setCurrentTime(&timeOpenFile);

    /* Wait for response */
    std::set<uint32_t> servers;
    while(!isTimeout(timeOpenFile, OPENFILE_TIMEOUT)) {
        PacketBase pb;
        N->recv(pb);
        /* Only handle OpenFileAck */
        if(pb.opCode == OPCODE_OPENFILEACK) {
            PacketOpenFile pof;
            pof.deserialize(pb.buf);
            servers.insert(pof.id);
            if(servers.size() >= numServers) {
                fileOpened = true;
                return ++curFd;
            }
        }
    }
    return -1;
}

int ClientInstance::writeBlock(int fd, char *strData, int byteOffset, int blockSize)
{
    return -1;
}

int ClientInstance::commit(int fd)
{
    return -1;
}

int ClientInstance::abort(int fd)
{
    return -1;
}

int ClientInstance::closeFile(int fd)
{
    return -1;
}

void ClientInstance::setCurrentTime(timeval *t)
{
    gettimeofday(t, NULL);
}

bool ClientInstance::isTimeout(timeval startup, long timeout)
{
    struct timeval cur;
    gettimeofday(&cur, NULL);
    if (timediff(cur, startup) > timeout) {
        return true;
    } else {
        return false;
    }
}

long ClientInstance::timediff(timeval t1, timeval t2)
{
    return (t1.tv_sec - t2.tv_sec) * 1000 + (t1.tv_usec - t2.tv_usec) / 1000;
}
