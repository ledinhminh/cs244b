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
    setCurrentTime(&timeOpenFileRetry);
    std::set<uint32_t> servers;

    /* Wait for response */
    while(!isTimeout(timeOpenFile, OPENFILE_TIMEOUT)) {
        PacketBase pb;
        if(N->recv(pb) > 0) {
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
        /* Retry if no response */
        if(isTimeout(timeOpenFileRetry, OPENFILE_RETRY_TIMEOUT)) {
            PRINT("***Resending OpenFile\n");
            N->send(p);
            setCurrentTime(&timeOpenFileRetry);
        }
    }
    return -1;
}

int ClientInstance::writeBlock(int fd, char *strData, int byteOffset, int blockSize)
{
    if(!fileOpened || (fileOpened && fd != curFd)) {
        return -1;
    }
    if(byteOffset < 0 || byteOffset > MAX_FILE_SIZE) {
        return -1;
    }
    if(strData == NULL) {
        return -1;
    }
    if(blockSize < 0 || blockSize > MAX_BLOCK_SIZE ) {
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
    while(true) {
        /* Prepare */
        PacketCommitPrepare p;
        p.fileID = curFd;
        p.numBlocks = blocks.size();
        for(mapit it = blocks.begin(); it != blocks.end(); ++it) {
            p.blockIDs.insert(it->first);
        }
        N->send(p);

        /* Wait for CommitReady || Resend */
        std::set<uint32_t> readyServers;
        setCurrentTime(&timeCommitPrepare);
        setCurrentTime(&timeCommitPrepareRetry);

        while(true) {
            PacketBase pb;
            if(N->recv(pb) > 0) {
                if(pb.opCode == OPCODE_COMMITREADY) {
                    PacketCommitReady pcr;
                    pcr.deserialize(pb.buf);
                    readyServers.insert(pcr.id);
                    if(readyServers.size() >= numServers) {
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
                    blockIDit it = prb.blockIDs.begin();
                    for(; it != prb.blockIDs.end(); ++it) {
                        mapit blk = blocks.find(*it);
                        if(blk == blocks.end()) {
                            throw FSException("Unknown blocks to resend");
                        }
                        N->send(blk->second);
                    }
                    break; //Restart CommitPrepare
                }
            }
            /* Retry CommitPrepare but keep response */
            if(isTimeout(timeCommitPrepareRetry, COMMITPREPARE_RETRY_TIMEOUT)) {
            PRINT("***Resending CommitPrepare\n");
                N->send(p);
                setCurrentTime(&timeCommitPrepareRetry);
            }
            /* CommitPrepare timeout. Exit */
            if(isTimeout(timeCommitPrepare, COMMITPREPARE_TIMEOUT)) {
                return -1;
            }
        }
    }
    return -1;
}

int ClientInstance::commitFinal()
{
    PacketCommit p;
    p.fileID = curFd;
    N->send(p);
    setCurrentTime(&timeCommit);
    setCurrentTime(&timeCommitRetry);
    std::set<uint32_t> successServers;
    while(!isTimeout(timeCommit, COMMIT_TIMEOUT)) {
        PacketBase pb;
        if(N->recv(pb) > 0) {
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
        /* Retry Commit */
        if(isTimeout(timeCommitRetry, COMMIT_RETRY_TIMEOUT)) {
            PRINT("***Resending Commit\n");
            N->send(p);
            setCurrentTime(&timeCommitRetry);
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
    PacketClose p;
    p.fileID = curFd;
    N->send(p);
    blocks.clear();

    setCurrentTime(&timeClose);
    setCurrentTime(&timeCloseRetry);
    std::set<uint32_t> servers;

    /* Wait for response */
    while(!isTimeout(timeClose, CLOSE_TIMEOUT)) {
        PacketBase pb;
        if(N->recv(pb) > 0) {
            /* Only handle OpenFileAck */
            if(pb.opCode == OPCODE_CLOSEACK) {
                PacketCloseAck pca;
                pca.deserialize(pb.buf);
                servers.insert(pca.id);
                if(servers.size() >= numServers) {
                    fileOpened = false;
                    return 0;
                }
            }
        }
        /* Retry if no response */
        if(isTimeout(timeCloseRetry, CLOSE_RETRY_TIMEOUT)) {
            N->send(p);
            setCurrentTime(&timeCloseRetry);
        }
    }
    return -1;
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
