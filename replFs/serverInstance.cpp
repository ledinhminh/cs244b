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
        PacketOpenFile p;
        p.deserialize(pb.buf);
        curFd=pb.fileID;  
        fileOpened=true;
        state = Write;
    }
}

void ServerInstance::handleWrite(PacketBase &pb)
{
    if(pb.opCode == OPCODE_WRITEBLOCK) {
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
