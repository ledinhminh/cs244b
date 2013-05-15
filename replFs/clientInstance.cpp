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
    /* Prepare */
    PacketCommitPrepare p;
    p.fileID = curFd;
    p.numBlocks = blocks.size();
    for(mapit it = blocks.begin(); it != blocks.end(); ++it) {
        p.blockIDs.push_back(it->first);
    }
    N->send(p);

    /* Wait for CommitReady || Resend */
    std::set<uint32_t> readyServers;
    setCurrentTime(&timeCommitPrepare);

    while(!isTimeout(timeCommitPrepare, COMMITPREPARE_TIMEOUT)) {
        PacketBase pb;
        if(N->recv(pb) < 0) {
            continue;
        }
        if(pb.opCode == OPCODE_COMMITREADY) {
            PacketCommitReady pcr;
            pcr.deserialize(pb.buf);
            readyServers.insert(pcr.id);
            if(readyServers.size() >= numServers) {
                PacketCommit pc;
                pc.fileID = curFd;
                N->send(pc);
                return commitFinal();
            }
        } else if(pb.opCode == OPCODE_RESENDBLOCK) {
            /* + Resend blocks
             * + Reset timer
             * + Reset readyServers
             * + Resend CommitPrepare */
            setCurrentTime(&timeCommitPrepare);
            readyServers.clear();
            PacketResendBlock prb;
            prb.deserialize(pb.buf);
            for(blockIDit it = prb.blockIDs.begin(); it != prb.blockIDs.end(); ++it) {
                mapit blk = blocks.find(*it);
                if(blk == blocks.end()) {
                    throw FSException("Unknown blocks to resend");
                }
                N->send(blk->second);
            }
            PacketCommitPrepare pcp;
            pcp.fileID = curFd;
            pcp.numBlocks = blocks.size();
            for(mapit it = blocks.begin(); it != blocks.end(); ++it) {
                pcp.blockIDs.push_back(it->first);
            }
            N->send(pcp);
        }
    }
    return -1;
}

int ClientInstance::commitFinal()
{
    setCurrentTime(&timeCommit);
    std::set<uint32_t> successServers;
    while(!isTimeout(timeCommit, COMMIT_TIMEOUT)) {
        PacketBase pb;
        if(N->recv(pb) < 0) {
            continue;
        }
        if(pb.opCode == OPCODE_COMMITSUCCESS) {
            PacketCommitSuccess pcs;
            pcs.deserialize(pb.buf);
            successServers.insert(pcs.id);
            if(successServers.size() >= numServers) {
                /* Clear block cache upon commit */
                blocks.clear();
                return 0;
            }
        }
    }
    return -1;
}

int ClientInstance::abort(int fd)
{
    if(!fileOpened || (fileOpened && fd != curFd)) {
        return -1;
    }
    PacketAbort p;
    p.fileID = curFd;
    N->send(p);
    blocks.clear();
    return 0;
}

int ClientInstance::closeFile(int fd)
{
    if(!fileOpened || (fileOpened && fd != curFd)) {
        return -1;
    }
    bool pendingCommit = blocks.size() > 0;
    if(pendingCommit) {
        commit(fd);
    }
    fileOpened = false;
    PacketClose p;
    p.fileID = curFd;
    N->send(p);
    blocks.clear();
    return 0;
}

/* ----------------- Private ----------------------- */

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
