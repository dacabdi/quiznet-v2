#include "Host.h"

Host::Host(const std::string& node, const std::string& service,
           const bool passive, AddressDomain addressDomain, 
           SocketType socketType)
{
    _node = node;
    _service = service;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = addressDomain;
    hints.ai_socktype = socketType;
    if(passive) 
    {
        hints.ai_flags = AI_PASSIVE; // suitable for bind
        _passive = true;
    }

    int r = getaddrinfo(node.c_str(), service.c_str(), &hints, &_addr);
    
    if(r) throw Exception(
        "Fetching address info (error:" + std::to_string(r) +
        ") " + std::string(gai_strerror(r)), "Host::Host()",
        std::string("node: ") + node + " service: " 
        + (service == "0" ? "Any" : service));
}

Host::Host(const std::string& service,
           AddressDomain addressDomain, 
           SocketType socketType)
{
    _node = "";
    _service = service;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = addressDomain;
    hints.ai_socktype = socketType;
    hints.ai_flags = AI_PASSIVE; // suitable for bind
    _passive = true;
    
    int r = getaddrinfo(NULL, service.c_str(), &hints, &_addr);
    
    if(r) throw Exception(
        "Fetching address info (error:" + std::to_string(r) +
        ") " + std::string(gai_strerror(r)), "Host::Host()",
        std::string("node: ") + _node + " service: " 
        + (service == "0" ? "Any" : service));
}

Host::Host(struct sockaddr *info)
{
    _addr = (struct addrinfo *)malloc(sizeof(*_addr));
    _addr->ai_addr = info;
    _node = getAddress();
    _created_on_ref = true;
    // TODO get port
}


const std::string& Host::getNode(void) const
{
    return _node;
}

const std::string Host::getAddress(void) const
{
    char str[256];

    if(inet_ntop(_addr->ai_addr->sa_family, 
              &((struct sockaddr_in *)_addr->ai_addr)->sin_addr, 
              str, 256) == NULL)
        throw Exception(
            "Failed to get address from addrinfo structure",
            "Host::getAddress()");

    return std::string(str);
}

const std::string Host::getAddressAndPort(void) const
{
    return getAddress() + ":" + std::to_string(getPort());
}

struct addrinfo* Host::getAddressInfo(void)
{
    return _addr;
}

const uint16_t Host::getPort(void) const
{
    return ntohs(getInPort());
}

const std::string& Host::getService(void) const
{
    return _service;
}

bool Host::isPassive(void) const
{
    return _passive;
}

in_port_t Host::getInPort(void) const
{
    return (((struct sockaddr_in*)(_addr->ai_addr))->sin_port);
}

Host::~Host()
{
    if(!_created_on_ref) 
        freeaddrinfo(_addr);
}