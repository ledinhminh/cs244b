#include "exception.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class NetworkInstance {
private:
    const unsigned short port;
    const unsigned int group;
    int socket;
    sockaddr_in myAddr;
public:
    NetworkInstance(unsigned short _port, unsigned int _group) :
        port(_port), group(_group) {};

    void init();

    int send();
    int recv();
};
