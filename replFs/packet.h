#ifndef PACKET_H
#define PACKET_H

//#debug NDEBUG

#include <assert.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>

#define OPCODE_OPENFILE     0x0
#define OPCODE_OPENFILEACK  0x1
#define OPCODE_WRITEBLOCK   0x2
#define OPCODE_COMMITPREPARE    0x3
#define OPCODE_RESENDBLOCK  0x4
#define OPCODE_COMMITREADY  0x5
#define OPCODE_COMMIT       0x6
#define OPCODE_COMMITSUCCESS   0x7
#define OPCODE_ABORT        0x8

#define TYPE_CLIENT 0
#define TYPE_SERVER 1

#define MAX_FILENAME_SIZE   128
#define MAX_NUM_BLOCKS      128
#define MAX_BLOCK_SIZE      512

struct Packet {
    uint8_t type;
    uint8_t body[32];
};

class PacketHeader {
public:
    PacketHeader() {
        zero = 0;
        version = 0;
    };
    uint8_t opCode;
    uint8_t zero;
    uint8_t version;
    uint8_t type;
    uint32_t id;
    uint32_t seqNum;
    uint32_t fileID;

    virtual void serialize(std::ostream &sink) {
        PacketHeader p(*this);
        p.id = htonl(id);
        p.seqNum = htonl(seqNum);
        p.fileID = htonl(fileID);
        sink.write(reinterpret_cast<char *>(&p.opCode), sizeof(uint8_t));
        sink.write(reinterpret_cast<char *>(&p.zero),   sizeof(uint8_t));
        sink.write(reinterpret_cast<char *>(&p.version), sizeof(uint8_t));
        sink.write(reinterpret_cast<char *>(&p.type),   sizeof(uint8_t));
        sink.write(reinterpret_cast<char *>(&p.id),     sizeof(uint32_t));
        sink.write(reinterpret_cast<char *>(&p.seqNum), sizeof(uint32_t));
        sink.write(reinterpret_cast<char *>(&p.fileID), sizeof(uint32_t));
    }

    virtual void deserialize(std::istream &source) {
        source.read(reinterpret_cast<char *>(&opCode),  sizeof(uint8_t));
        source.read(reinterpret_cast<char *>(&zero),    sizeof(uint8_t));
        source.read(reinterpret_cast<char *>(&version), sizeof(uint8_t));
        source.read(reinterpret_cast<char *>(&type),    sizeof(uint8_t));
        source.read(reinterpret_cast<char *>(&id),      sizeof(uint32_t));
        source.read(reinterpret_cast<char *>(&seqNum),  sizeof(uint32_t));
        source.read(reinterpret_cast<char *>(&fileID),  sizeof(uint32_t));
        id = ntohl(id);
        seqNum = ntohl(seqNum);
        fileID = ntohl(fileID);
    }

};

class PacketOpenFile: public PacketHeader {
public:
    PacketOpenFile() {
        opCode = OPCODE_OPENFILE;
        type = TYPE_CLIENT;
    }
    char filename[MAX_FILENAME_SIZE];

    virtual void serialize(std::ostream &sink) {
        PacketHeader::serialize(sink);
        sink.write(reinterpret_cast<char *>(&filename), sizeof(filename));
    }
    virtual void deserialize(std::istream &source) {
        PacketHeader::deserialize(source);
        source.read(reinterpret_cast<char *>(&filename), sizeof(filename));
        filename[MAX_FILENAME_SIZE - 1] = '\0';
    }
};

class PacketOpenFileAck: public PacketHeader {
public:
    PacketOpenFileAck() {
        opCode = OPCODE_OPENFILEACK;
        type = TYPE_SERVER;
    }
    uint8_t status;

    virtual void serialize(std::ostream &sink) {
        PacketHeader::serialize(sink);
        sink.write(reinterpret_cast<char *>(&status), sizeof(uint8_t));
    }

    virtual void deserialize(std::istream &source) {
        PacketHeader::deserialize(source);
        source.read(reinterpret_cast<char *>(&status),  sizeof(uint8_t));
    }
};

class PacketWriteBlock: public PacketHeader {
public:
    PacketWriteBlock() {
        opCode = OPCODE_WRITEBLOCK;
        type = TYPE_CLIENT;
    }

    /* Copy constructor to handle payload clone */
    PacketWriteBlock(const PacketWriteBlock &other) : PacketHeader(other) {
        blockID = other.blockID;
        offset = other.offset;
        size = other.size;
        payload << other.payload.rdbuf();
    }

    uint32_t blockID;
    uint32_t offset;
    uint32_t size;
    std::stringstream payload;

    virtual void serialize(std::ostream &sink) {
        PacketHeader::serialize(sink);
        PacketWriteBlock p(*this);
        p.blockID = htonl(blockID);
        p.offset = htonl(offset);
        p.size = htonl(size);
        assert(size <= MAX_BLOCK_SIZE);
        sink.write(reinterpret_cast<char *>(&p.blockID), sizeof(uint32_t));
        sink.write(reinterpret_cast<char *>(&p.offset), sizeof(uint32_t));
        sink.write(reinterpret_cast<char *>(&p.size), sizeof(uint32_t));
        sink << payload.rdbuf();
    }

    virtual void deserialize(std::istream &source) {
        PacketHeader::deserialize(source);
        source.read(reinterpret_cast<char *>(&blockID), sizeof(uint32_t));
        source.read(reinterpret_cast<char *>(&offset),  sizeof(uint32_t));
        source.read(reinterpret_cast<char *>(&size),    sizeof(uint32_t));
        payload << source.rdbuf();
        blockID = ntohl(blockID);
        offset = ntohl(offset);
        size = ntohl(size);
        assert(size <= MAX_BLOCK_SIZE);
    }
};

class PacketCommitPrepare: public PacketHeader {
public:
    PacketCommitPrepare() {
        opCode = OPCODE_COMMITPREPARE;
        type = TYPE_CLIENT;
    }

    uint32_t numBlocks;
    std::vector<uint32_t> blockIDs;

    virtual void serialize(std::ostream &sink) {
        PacketHeader::serialize(sink);
        PacketCommitPrepare p(*this);
        p.numBlocks = htonl(blockIDs.size());
        assert(blockIDs.size() <= MAX_NUM_BLOCKS);
        sink.write(reinterpret_cast<char *>(&p.numBlocks), sizeof(uint32_t));
        std::vector<uint32_t>::iterator it;
        for(it = blockIDs.begin(); it != blockIDs.end(); ++it) {
            uint32_t bid = htonl(*it);
            sink.write(reinterpret_cast<char *>(&bid), sizeof(uint32_t));
        }
    }

    virtual void deserialize(std::istream &source) {
        PacketHeader::deserialize(source);
        source.read(reinterpret_cast<char *>(&numBlocks), sizeof(uint32_t));
        numBlocks = ntohl(numBlocks);
        assert(numBlocks <= MAX_NUM_BLOCKS);
        for(unsigned int i = 0; i < numBlocks; i++) {
            uint32_t bid;
            source.read(reinterpret_cast<char *>(&bid),  sizeof(uint32_t));
            blockIDs.push_back(ntohl(bid));
        }
    }
};

class PacketResendBlock: public PacketCommitPrepare {
public:
    PacketResendBlock() {
        opCode = OPCODE_RESENDBLOCK;
        type = TYPE_SERVER;
    }
};

class PacketCommitReady: public PacketHeader {
public:
    PacketCommitReady() {
        opCode = OPCODE_COMMITREADY;
        type = TYPE_SERVER;
    }
};

class PacketCommit: public PacketHeader {
public:
    PacketCommit() {
        opCode = OPCODE_COMMIT;
        type = TYPE_CLIENT;
    }
};

class PacketCommitSuccess: public PacketHeader {
public:
    PacketCommitSuccess() {
        opCode = OPCODE_COMMITSUCCESS;
        type = TYPE_SERVER;
    }
};

class PacketAbort: public PacketHeader {
public:
    PacketAbort() {
        opCode = OPCODE_ABORT;
        type = TYPE_CLIENT;
    }
};
#endif