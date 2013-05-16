#ifndef NETWORKINSTANCE_H
#define NETWORKINSTANCE_H

#include "strings.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <map>

#include "exception.h"
#include "packet.h"

#define FS_GROUP 0xE0010101
#define POLL_INTERVAL 100

class NetworkInstance {
private:
    const unsigned short port;
    const unsigned int group;
    const int droprate;

    int pollTimeout;
    int mySocket;
    sockaddr_in myAddr;

    uint32_t seqNum;
    uint32_t id;

    std::map<uint32_t, uint32_t> seqMap;

public:
    char hostname[16];

    NetworkInstance(unsigned short _port, unsigned int _group,
                    int _droprate, bool isBlocking) :
        port(_port), group(_group), droprate(_droprate) {
        initSocket();
        timeval ct;
        gettimeofday(&ct, NULL);
        srand(ct.tv_usec);
        id = rand();
        seqNum = 2;
        if(isBlocking) {
            pollTimeout = -1;
        } else {
            pollTimeout = POLL_INTERVAL;
        }
        gethostname(hostname, sizeof(hostname));
        hostname[sizeof(hostname)-1]='\0';
    };
    void send(PacketBase &p);
    int recv(PacketBase &p);
private:
    bool hasData();
    void initSocket();
    bool isDropped();
};

#endif
