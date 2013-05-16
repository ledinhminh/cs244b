#include "serverInstance.h"

void ServerInstance::run()
{
    while(true) {
        PacketBase pb;
        if(N->recv(pb) == 0) {
            continue;
        }
        if(pb.type == TYPE_SERVER) {
            continue;
        }
        switch(state) {
        case Idle:
            handleIdle(pb);
            break;
        case Write:
            handleWrite(pb);
            break;
        case CommitReady:
            handleCommitReady(pb);
            break;
        default:
            throw FSException("Unknown server state");
        }
    }
}

void ServerInstance::transit(enum serverState next)
{
    lastState = state;
    state = next;
}

void ServerInstance::handleIdle(PacketBase &pb)
{
    if(pb.opCode == OPCODE_OPENFILE) {
        PacketOpenFile p;
        p.deserialize(pb.buf);
        curFd = p.fileID;
        filepath = mount + "/" + p.filename;

        PacketOpenFileAck pr;
        pr.fileID = curFd;

        curFile = fopen(filepath.c_str(), "r+b");
        if(curFile == NULL) {
            newFile = true;
        } else {
            newFile = false;
        }
        PRINT("Opening file: %s\n", filepath.c_str());
        pr.status = FILEOPENACK_OK;
        transit(Write);
        N->send(pr);
    } else if(pb.opCode == OPCODE_CLOSE) {
        /* Missed Close request */
        PacketClose p;
        p.deserialize(pb.buf);
        if(p.fileID == closedFd) {
            PacketCloseAck pca;
            pca.fileID = closedFd;
            N->send(pca);
        }
    }
}

void ServerInstance::handleWrite(PacketBase &pb)
{
    if(pb.opCode == OPCODE_OPENFILE) {
        if(pb.fileID == curFd && lastState == Idle) {
            PacketOpenFileAck pr;
            pr.fileID = curFd;
            pr.status = FILEOPENACK_OK;
            N->send(pr);
        }
    } else if(pb.opCode == OPCODE_WRITEBLOCK) {
        PacketWriteBlock p;
        p.deserialize(pb.buf);
        if(p.fileID != curFd) {
            throw FSException("Unknown file descriptor");
        }
        blocks[p.blockID] = p;
    } else if(pb.opCode == OPCODE_COMMITPREPARE) {
        PacketCommitPrepare p;
        p.deserialize(pb.buf);
        if(p.fileID != curFd) {
            throw FSException("Unknown file descriptor");
        }
        /* Clean up extra blocks due to missed abort */
        mapit mit = blocks.begin();
        while(mit != blocks.end()) {
            if(p.blockIDs.count(mit->first) == 0) {
                mapit toErase = mit;
                ++mit;
                blocks.erase(toErase);
            } else {
                ++mit;
            }
        }
        /* Find missing blocks */
        PacketResendBlock prb;
        for(blockIDit it = p.blockIDs.begin(); it != p.blockIDs.end(); ++it) {
            if(blocks.find(*it) == blocks.end()) {
                prb.blockIDs.insert(*it);
            }
        }
        if(prb.blockIDs.size() > 0) {
            /* Missing blocks; ask for resend */
            prb.fileID = curFd;
            N->send(prb);
            PRINT("[%s] Missing %lu blocks\n", N->hostname, prb.blockIDs.size());
        } else {
            /* I'm ready */
            PacketCommitReady pcr;
            pcr.fileID = curFd;
            N->send(pcr);
            transit(CommitReady);
            PRINT("[%s] Ready to commit\n", N->hostname);
        }
    } else if(pb.opCode == OPCODE_ABORT) {
        PRINT("FileID[%d] %lu writes aborted!\n", curFd, blocks.size());
        blocks.clear();
    } else if(pb.opCode == OPCODE_CLOSE) {
        /* Close file; Clear block cache; Go to Idle */
        blocks.clear();
        if(curFile) fclose(curFile);
        curFile = NULL;
        transit(Idle);
        PacketCloseAck pca;
        pca.fileID = closedFd = curFd;
        N->send(pca);
        PRINT("FileID[%d] closed.\n", curFd);
    } else if(pb.opCode == OPCODE_COMMIT) {
        if(lastState == CommitReady) {
            PRINT("[%s] Re-ack commitSuccess\n", N->hostname);
            PacketCommitSuccess pcs;
            pcs.fileID = curFd;
            N->send(pcs);
        }
    }
}

void ServerInstance::handleCommitReady(PacketBase &pb)
{
    if(pb.opCode == OPCODE_COMMIT) {
        /* Flush blocks to disk (sort by client order) */
        PRINT("[%s] FileID[%d] flushing writes...\n", N->hostname, curFd);
        for(mapit it = blocks.begin(); it != blocks.end(); ++it) {
            PacketWriteBlock &blk = it->second;
            if(newFile) {
                curFile = fopen(filepath.c_str(), "wb");
                if(curFile == NULL) {
                    throw FSException("Error creating empty file.");
                }
                newFile = false;
            }
            fseek(curFile, blk.offset, SEEK_SET);
            fwrite(blk.payload.str().c_str(), 1, blk.size, curFile);
            fflush(curFile);
        }
        PRINT("[%s] FileID[%d] %lu writes committed!\n", 
                N->hostname, curFd, blocks.size());
        PacketCommitSuccess pcs;
        pcs.fileID = curFd;
        N->send(pcs);
        /* Clear block cache and wait for more writes */
        blocks.clear();
        transit(Write);
    } else if(pb.opCode == OPCODE_WRITEBLOCK) {
        /* Mostly like other servers' resend response */
        PacketWriteBlock p;
        p.deserialize(pb.buf);
        if(p.fileID != curFd) {
            throw FSException("Unknown file descriptor");
        }
        blocks[p.blockID] = p;
        //transit(Write);
    } else if(pb.opCode == OPCODE_COMMITPREPARE) {
        PacketCommitReady pcr;
        pcr.fileID = curFd;
        N->send(pcr);
        PRINT("Ready to commit\n");
    } else if(pb.opCode == OPCODE_CLOSE) {
        /* Close file; Clear block cache; Go to Idle */
        blocks.clear();
        if(curFile) fclose(curFile);
        curFile = NULL;
        transit(Idle);
        PacketCloseAck pca;
        pca.fileID = closedFd = curFd;
        N->send(pca);
        PRINT("FileID[%d] closed.\n", curFd);
    }
}
