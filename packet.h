#ifndef PACKET_H
#define PACKET_H

#include <map>
#include "Exception.h"

#define TYPE_HEARTBEAT      0x0
#define TYPE_NAME_REQUEST   0x1
#define TYPE_NAME_RESPONSE  0x2

struct MW244BPacket {
    uint8_t type;
    uint8_t body[32];
};

class mazePacket {
public:
    mazePacket() {
        zero = 0;
        version = 0;
    };
    uint8_t type;
    uint8_t zero;
    uint16_t version;
    uint32_t id;
    uint32_t seqNum;

    virtual size_t size() const {
        return 12;
    };

    virtual void serialize(uint8_t *buf, size_t size) {
        int offset = 0;
        mazePacket n;
        n.type = type;
        n.zero = zero;
        n.version = htons(version);
        n.id = htonl(id);
        n.seqNum = htonl(seqNum);
        if(size < this->size()) {
            return;
        }
        memcpy(buf + offset, &(n.type), sizeof(n.type));
        offset += sizeof(n.type);
        memcpy(buf + offset, &n.zero, sizeof(n.zero));
        offset += sizeof(n.zero);
        memcpy(buf + offset, &n.version, sizeof(n.version));
        offset += sizeof(n.version);
        memcpy(buf + offset, &(n.id), sizeof(n.id));
        offset += sizeof(n.id);
        memcpy(buf + offset, &n.seqNum, sizeof(n.seqNum));
        offset += sizeof(n.seqNum);
    }

    virtual void deserialize(uint8_t *buf, size_t size) {
        int offset = 0;
        if(size < this->size()) {
            return;
        }
        memcpy(&type, buf + offset, sizeof(type));
        offset += sizeof(type);
        memcpy(&zero, buf + offset, sizeof(zero));
        offset += sizeof(zero);
        memcpy(&version, buf + offset, sizeof(version));
        offset += sizeof(version);
        memcpy(&id, buf + offset, sizeof(id));
        offset += sizeof(id);
        memcpy(&seqNum, buf + offset, sizeof(seqNum));
        offset += sizeof(seqNum);

        version = ntohs(version);
        id = ntohl(id);
        seqNum = ntohl(seqNum);
    }
};

class heartbeat : public mazePacket {
public:
    heartbeat() {
        type = TYPE_HEARTBEAT;
    }
    int8_t xLoc;
    int8_t yLoc;
    int8_t xMis;
    int8_t yMis;
    uint8_t seqMis;
    uint8_t dir;
    uint16_t score;
    
    bool hasMissile(){
        return (xMis>=0 && yMis>=0);
    }

    virtual size_t size() const {
        return mazePacket::size() + 8;
    }

    virtual void serialize(uint8_t *buf, size_t size) {
        int offset = mazePacket::size();
        heartbeat n;
        n.xLoc = xLoc;
        n.yLoc = yLoc;
        n.xMis = xMis;
        n.yMis = yMis;
        n.seqMis = seqMis;
        n.dir = dir;
        n.score = htons(score);
        if(size < this->size() + offset) {
            return;
        }
        mazePacket::serialize(buf, size);
        memcpy(buf + offset, &n.xLoc, sizeof(n.xLoc));
        offset += sizeof(n.xLoc);
        memcpy(buf + offset, &n.yLoc, sizeof(n.yLoc));
        offset += sizeof(n.yLoc);
        memcpy(buf + offset, &n.xMis, sizeof(n.xMis));
        offset += sizeof(n.xMis);
        memcpy(buf + offset, &n.yMis, sizeof(n.yMis));
        offset += sizeof(n.yMis);
        memcpy(buf + offset, &n.seqMis, sizeof(n.seqMis));
        offset += sizeof(n.seqMis);
        memcpy(buf + offset, &n.dir, sizeof(n.dir));
        offset += sizeof(n.dir);
        memcpy(buf + offset, &n.score, sizeof(n.score));
        offset += sizeof(n.score);
    }

    virtual void deserialize(uint8_t *buf, size_t size) {
        int offset = mazePacket::size();
        if(size < this->size() + offset) {
            return;
        }
        mazePacket::deserialize(buf, size);
        memcpy(&xLoc, buf + offset, sizeof(xLoc));
        offset += sizeof(xLoc);
        memcpy(&yLoc, buf + offset, sizeof(yLoc));
        offset += sizeof(yLoc);
        memcpy(&xMis, buf + offset, sizeof(xMis));
        offset += sizeof(xMis);
        memcpy(&yMis, buf + offset, sizeof(yMis));
        offset += sizeof(yMis);
        memcpy(&seqMis, buf + offset, sizeof(seqMis));
        offset += sizeof(seqMis);
        memcpy(&dir, buf + offset, sizeof(dir));
        offset += sizeof(dir);
        memcpy(&score, buf + offset, sizeof(score));
        offset += sizeof(score);
        score = ntohs(score);
    }
};

class nameRequest : public mazePacket {
public:
    nameRequest() {
        type = TYPE_NAME_REQUEST;
    }
    uint32_t targetID;
    virtual size_t size() const {
        return mazePacket::size() + 4;
    }
};

class nameResponse : public mazePacket {
public:
    nameResponse() {
        type = TYPE_NAME_RESPONSE;
    }
    char name[20];
    virtual size_t size() const {
        return mazePacket::size() + 20;
    }
};

#endif
