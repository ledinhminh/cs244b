#include "packet.h"

const char *PacketBase::opCodeStr[] = {
    "OpenFile",
    "OpenFileAck",
    "WriteBlock",
    "CommitPrepare",
    "ResendBlock",
    "CommitReady",
    "Commit",
    "CommitSuccess",
    "Abort",
    "Close",
};
