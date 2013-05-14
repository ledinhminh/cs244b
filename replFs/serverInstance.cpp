#include "serverInstance.h"

void ServerInstance::run()
{
    while(true) {
        PacketBase pb;
        if(N->recv(pb) < 0) {
            throw FSException("Server non-blocking!");
        }
        if(pb.type == TYPE_SERVER) {
            continue;
        }
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
        curFd=p.fileID;
        filename.append("/");
        filename.append(p.filename);
        
        PacketOpenFileAck pr;
        pr.fileID=curFd;
        
        curFile=fopen(filename.c_str(),"w");
        PRINT("Opening file: %s\t", filename.c_str());
        if(curFile == NULL){
            PRINT("FAIL\n");
            pr.status=FILEOPENACK_FAIL;
        }else{
            PRINT("OK\n");
            pr.status=FILEOPENACK_OK;
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
        p.print();
        if(p.fileID!=curFd){
            throw FSException("Unknown file descriptor");
        }
        blocks[p.blockID]=p; 
    } else if(pb.opCode == OPCODE_COMMITPREPARE) {
    } else if(pb.opCode == OPCODE_ABORT) {
    } else if(pb.opCode == OPCODE_CLOSE) {
    }
}

void ServerInstance::handleCommitReady(PacketBase &pb)
{
    if(pb.opCode == OPCODE_COMMIT) {
    } else if(pb.opCode == OPCODE_CLOSE) {
    }
}
