#include "strings.h"
#include "stdlib.h"
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "exception.h"
#include "packet.h"

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
    int send(PacketBase &p);
    int recv(std::istream &s);
private:
    void initSocket();
    sockaddr_in *resolveHost(register char *name);
};
