#ifndef __HOST__H__
#define __HOST__H__

#include "Exception.h"

#include "IHost.h"
#include "ISocket.h"

#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Host : public IHost
{
    public:

        Host(const std::string& node, 
             const std::string& service, // port
             bool passive = false,
             AddressDomain addressDomain = IPv4, 
             SocketType socketType = StreamSocket);

        Host(const std::string& service, // port
             AddressDomain addressDomain = IPv4, 
             SocketType socketType = StreamSocket);

        Host(struct sockaddr* info);

        const std::string& getNode(void) const override;
        const std::string getAddress(void) const override;
        const uint16_t getPort(void) const override;
        const std::string getAddressAndPort(void) const override;
        struct addrinfo* getAddressInfo(void) override;
        const std::string& getService(void) const override;
        bool isPassive(void) const override;

        ~Host();

    protected:

        std::string _node;
        std::string _service;
        struct addrinfo *_addr;
        bool _passive = false;
        bool _created_on_ref = false;

        in_port_t getInPort(void) const;

        Host(){};
};

#endif // __HOST__H__

