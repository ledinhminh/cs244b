#include "serverInstance.h"

void ServerInstance::run()
{
    while(true) {
        PacketBase pb;
        if(N->recv(pb) < 0) {
            throw FSException("Server non-blocking!");
        }
        //TODO: check seqNum
        if(pb.type == TYPE_SERVER) {
            continue;
        }
        PRINT("Processing packet...\t");
        pb.print();
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

void ServerInstance::handleIdle(PacketBase &pb)
{
    if(pb.opCode == OPCODE_OPENFILE) {
        std::string filename(mount);
        PacketOpenFile p;
        p.deserialize(pb.buf);
        curFd = p.fileID;
        filename.append("/");
        filename.append(p.filename);

        PacketOpenFileAck pr;
        pr.fileID = curFd;

        curFile = fopen(filename.c_str(), "w");
        PRINT("Opening file: %s\t", filename.c_str());
        if(curFile == NULL) {
            PRINT("FAIL\n");
            pr.status = FILEOPENACK_FAIL;
        } else {
            PRINT("OK\n");
            pr.status = FILEOPENACK_OK;
            state = Write;
        }
        N->send(pr);
    }
}

void ServerInstance::handleWrite(PacketBase &pb)
{
    if(pb.opCode == OPCODE_WRITEBLOCK) {
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
        PacketResendBlock prb;
        for(blockIDit it = p.blockIDs.begin(); it != p.blockIDs.end(); ++it) {
            if(blocks.find(*it) == blocks.end()) {
                prb.blockIDs.push_back(*it);
            }
        }
        if(prb.blockIDs.size() > 0) {
            /* Missing blocks; ask for resend */
            prb.fileID = curFd;
            N->send(prb);
            PRINT("Missing %lu blocks\n", prb.blockIDs.size());
        } else {
            /* I'm ready */
            PacketCommitReady pcr;
            pcr.fileID = curFd;
            N->send(pcr);
            state = CommitReady;
            PRINT("Ready to commit\n");
        }
    } else if(pb.opCode == OPCODE_ABORT) {
        PRINT("FileID[%d] %lu writes aborted!\n", curFd, blocks.size());
        blocks.clear();
    } else if(pb.opCode == OPCODE_CLOSE) {
        /* Close file; Clear block cache; Go to Idle */
        blocks.clear();
        fclose(curFile);
        curFile = NULL;
        state = Idle;
        PRINT("FileID[%d] closed.\n", curFd);
    }
}

void ServerInstance::handleCommitReady(PacketBase &pb)
{
    if(pb.opCode == OPCODE_COMMIT) {
        /* Flush blocks to disk */
        for(mapit it = blocks.begin(); it != blocks.end(); ++it) {
            PacketWriteBlock &blk = it->second;
            fseek(curFile, blk.offset, SEEK_SET);
            fwrite(blk.payload.str().c_str(), 1, blk.size, curFile);
        }
        PRINT("FileID[%d] %lu writes committed!\n", curFd, blocks.size());
        PacketCommitSuccess pcs;
        pcs.fileID = curFd;
        N->send(pcs);
        /* Clear block cache and wait for more writes */
        blocks.clear();
        state = Write;
    } else if(pb.opCode == OPCODE_WRITEBLOCK) {
        /* Mostly like other servers' resend response */
        PacketWriteBlock p;
        p.deserialize(pb.buf);
        if(p.fileID != curFd) {
            throw FSException("Unknown file descriptor");
        }
        blocks[p.blockID] = p;
        state = Write;
    } else if(pb.opCode == OPCODE_CLOSE) {
        /* Close file; Clear block cache; Go to Idle */
        blocks.clear();
        fclose(curFile);
        curFile = NULL;
        state = Idle;
        PRINT("FileID[%d] closed.\n", curFd);
    }
}
