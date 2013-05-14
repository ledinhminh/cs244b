#include "strings.h"
#include "stdlib.h"
#include "string.h"
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "exception.h"
#include "packet.h"

#define FS_GROUP 0xE0010101

class NetworkInstance {
private:
    const unsigned short port;
    const unsigned int group;
    const int droprate;

    int mySocket;
    sockaddr_in myAddr;

    uint32_t seqNum;
    uint32_t id;

public:
    NetworkInstance(unsigned short _port, unsigned int _group, int _droprate) :
        port(_port), group(_group), droprate(_droprate) {
        initSocket();
        id = rand();
        seqNum = 0;
    };
    void send(PacketBase &p);
    int recv(PacketBase &p);
private:
    void initSocket();
    bool isDropped();
    sockaddr_in *resolveHost(register char *name);
};
