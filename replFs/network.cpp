#include "network.h"

void NetworkInstance::init()
{
    sockaddr_in		nullAddr;
    sockaddr_in		*thisHost;
    char			buf[128];
    int				reuse;
    u_char          ttl;
    struct ip_mreq  mreq;

    gethostname(buf, sizeof(buf));
    if ((thisHost = resolveHost(buf)) == (sockaddr_in *) NULL) {
        throw FSException("who am I?");
    }
    bcopy((caddr_t) thisHost, (caddr_t) &myAddr, sizeof(sockaddr_in));

    if((socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw FSException("can't get socket");
    }

    /* SO_REUSEADDR allows more than one binding to the same
       socket - you cannot have more than one player on one
       machine without this */
    reuse = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        FSException("setsockopt failed (SO_REUSEADDR)");
    }

    nullAddr.sin_family = AF_INET;
    nullAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    nullAddr.sin_port = htons(port);
    if (bind(socket, (struct sockaddr *)&nullAddr, sizeof(nullAddr)) < 0) {
        FSException("netInit binding failed.");
    }

    /* Multicast TTL:
       0 restricted to the same host
       1 restricted to the same subnet
       32 restricted to the same site
       64 restricted to the same region
       128 restricted to the same continent
       255 unrestricted

       DO NOT use a value > 32. If possible, use a value of 1 when
       testing.
    */

    ttl = 1;
    if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        FSException("setsockopt failed (IP_MULTICAST_TTL)");
    }

    /* join the multicast group */
    mreq.imr_multiaddr.s_addr = htonl(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (char *)&mreq, sizeof(mreq)) < 0) {
        FSException("setsockopt failed (IP_ADD_MEMBERSHIP)");
    }

}
