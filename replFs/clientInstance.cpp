#include "clientInstance.h"

int ClientInstance::openFile(char *strFileName)
{
    if(fileOpened || strlen(strFileName) >= MAX_FILENAME_SIZE) {
        return -1;
    }
    /* Send request */
    PacketOpenFile p;
    p.fileID = ++curFd;
    strcpy(p.filename, strFileName);
    N->send(p);
    setCurrentTime(&timeOpenFile);

    /* Wait for response */
    while(!isTimeout(timeOpenFile, OPENFILE_TIMEOUT)) {
        PacketBase pb;
        if(N->recv(pb) < 0) {
            continue;
        }
        /* Only handle OpenFileAck */
        if(pb.opCode == OPCODE_OPENFILEACK) {
            PacketOpenFile pof;
            pof.deserialize(pb.buf);
            servers.insert(pof.id);
            if(servers.size() >= numServers) {
                fileOpened = true;
                return curFd;
            }
        }
    }
    return -1;
}

int ClientInstance::writeBlock(int fd, char *strData, int byteOffset, int blockSize)
{
    if(!fileOpened || (fileOpened && fd != curFd)) {
        return -1;
    }
    if(byteOffset < 0 || byteOffset >= MAX_FILE_SIZE) {
        return -1;
    }
    if(strData == NULL) {
        return -1;
    }
    if(blockSize < 0 || blockSize >= MAX_BLOCK_SIZE ) {
        return -1;
    }
    PacketWriteBlock p;
    p.fileID = curFd;
    p.blockID = curBlockID++;
    p.offset = byteOffset;
    p.size = blockSize;
    p.payload.write(strData, blockSize);
    blocks[p.blockID] = p;
    N->send(p);

    return 0;
}

int ClientInstance::commit(int fd)
{
    if(!fileOpened || (fileOpened && fd != curFd)) {
        return -1;
    }

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
