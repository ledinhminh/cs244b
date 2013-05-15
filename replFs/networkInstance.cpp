#include "networkInstance.h"

void NetworkInstance::send(PacketBase &p)
{
    const char *buf;
    int packetSize;
    p.seqNum = seqNum++;
    p.id = id;
    std::stringstream sink;
    p.serialize(sink);
    buf=sink.str().c_str();
    packetSize=sink.str().length();
    if(isDropped()) {
        PRINT("Packet dropped\n");
        p.print();
        return;
    }
    PRINT("Sending packet...\t");
    p.print();
    for(int i=0;i<packetSize;i++){
        //PRINT("%02hhX", buf[i]);
    }
    if (sendto(mySocket, buf, packetSize, 0,
               (sockaddr *) &myAddr, sizeof(sockaddr_in)) < 0) {
        throw FSException("Send error");
    }

}

int NetworkInstance::recv(PacketBase &p)
{
    if(!hasData()) {
        return -1;
    }
    sockaddr fromAddr;
    socklen_t fromlen = sizeof(fromAddr);
    char buf[2048];
    int ret;
    ret = recvfrom(mySocket, buf, sizeof(buf), 0, &fromAddr, &fromlen);
    if(ret < 0) {
        throw FSException("Recv error");
    } else {
        //PRINT("Recvd packet size %d...", ret);
        std::stringstream source;
        source.write(buf, ret);
        for(unsigned int i=0;i<source.str().length();i++){
            //PRINT("%02hhX", source.str()[i]);
        }
        p.deserialize(source);
        return 0;
    }
}

bool NetworkInstance::hasData()
{
    int ret;
    struct pollfd udp;
    udp.fd = mySocket;
    udp.events = POLLIN;
    ret = poll(&udp, 1, pollTimeout);
    if(ret < 0) {
        throw FSException("Poll error");
    } else {
        if(udp.revents & POLLIN) {
            return true;
        } else {
            return -false;
        }
    }
}

void NetworkInstance::initSocket()
{
    sockaddr_in		nullAddr;
    int				reuse;
    u_char          ttl;
    struct ip_mreq  mreq;

    if((mySocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw FSException("can't get socket");
    }

    /* SO_REUSEADDR allows more than one binding to the same
       socket - you cannot have more than one player on one
       machine without this */
    reuse = 1;
    if (setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        FSException("setsockopt failed (SO_REUSEADDR)");
    }

    nullAddr.sin_family = AF_INET;
    nullAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    nullAddr.sin_port = htons(port);
    if (bind(mySocket, (struct sockaddr *)&nullAddr, sizeof(nullAddr)) < 0) {
        FSException("netInit binding failed.");
    }

    /* Multicast TTL: DO NOT use a value > 32. Use 1 when testing. */

    ttl = 1;
    if (setsockopt(mySocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        FSException("setsockopt failed (IP_MULTICAST_TTL)");
    }

    /* join the multicast group */
    mreq.imr_multiaddr.s_addr = htonl(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(mySocket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (char *)&mreq, sizeof(mreq)) < 0) {
        FSException("setsockopt failed (IP_ADD_MEMBERSHIP)");
    }

    memcpy(&myAddr, &nullAddr, sizeof(sockaddr_in));
    myAddr.sin_addr.s_addr = htonl(FS_GROUP);
    //fcntl(mySocket, F_SETFL, fcntl(mySocket, F_GETFL, 0) | O_NONBLOCK);
}

bool NetworkInstance::isDropped()
{
    return ((unsigned int)rand() % 100) < droprate;
}
