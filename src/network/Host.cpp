#include "Host.h"
#include <iostream>

Host::Host(const char * const ip, const char * const port)
{
    struct addrinfo * result;

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if(!ip || std::string(ip) == "0.0.0.0") hints.ai_flags = AI_PASSIVE;
    else hints.ai_flags = AI_CANONNAME;

    int r = getaddrinfo(ip, port, &hints, &result);
    
    if(r) throw Except("Failed to fetch host address information",
                       "Host::Host()", "Provided host information : " 
                       + (ip   ? std::string(ip)   : "nullptr") + ":"
                       + (port ? std::string(port) : "nullptr") +
                       + " | gai_strerror: " + gai_strerror(r), false);

    // save canonical name
    if(result->ai_canonname) _canonname = std::string(result->ai_canonname);
    
    for(struct addrinfo * p = result; p != nullptr; p = p->ai_next)
        _addr.push_back(*((struct sockaddr_in *)(p->ai_addr)));

    if(hints.ai_flags == AI_PASSIVE)
    {
        std::vector<struct sockaddr_in> ifs = Host::hostIfs();
        _addr.insert(_addr.end(), ifs.begin(), ifs.end());
    }

    freeaddrinfo(result);
}

Host::Host(const std::string& ip, const std::string& port)
: Host(ip.c_str(), port.c_str()) {}

Host::Host(struct sockaddr_in ref)
{
    bzero(ref.sin_zero, 8);
    _addr.clear();
    _addr.push_back(ref);

    if(ref.sin_addr.s_addr == INADDR_ANY) 
    {
        for(struct sockaddr_in i : Host::hostIfs())    
        {
            i.sin_port = ref.sin_port;
            _addr.push_back(i);
        }
    }
}

// default constructor
Host::Host(void) {} 

std::string Host::ip(size_t entry) const
{
    if(_addr.size() > entry)
    {
        char buff[16]; // |xxx.xxx.xxx.xxx\0| = 16

        const struct sockaddr_in &ref = _addr[entry];
        if(inet_ntop(ref.sin_family, &ref.sin_addr, buff, sizeof(ref)) == nullptr)
            throw Except("Failed to get IP from sockaddr_in structure", "Host::ip()");

        return std::string(buff);
    }
    
    throw Except("Host does not contain entry " 
                + std::to_string(entry), "Host::ip()", "", false);
}

uint16_t Host::port(size_t entry) const
{   
    if(_addr.size() > entry)
    {
        const struct sockaddr_in &ref = _addr[entry];
        return ntohs(ref.sin_port);
    }
    
    throw Except("Host does not contain entry " 
                + std::to_string(entry), "Host::port()", "", false);
}

std::string Host::fullName(size_t entry) const
{
    return ip(entry) + ":" + std::to_string(port(entry));
}

struct sockaddr_in Host::address(size_t entry) const
{
    if(_addr.size() > entry) return _addr[entry];
    throw Except("Host does not contain entry " 
                + std::to_string(entry), "Host::address()", "", false);
}

std::string Host::canonName(void) const
{
    return _canonname;
}

size_t Host::count(void) const
{
    return _addr.size();
}

std::vector<struct sockaddr_in> Host::hostIfs(void)
{
    std::vector<struct sockaddr_in> interfaces;

    struct ifaddrs * ifs = nullptr;
    int r = getifaddrs(&ifs);
    
    if(r) 
    {
        if (ifs != nullptr) freeifaddrs(ifs);
        throw Except("Failed to fetch host's interfaces on passive mode", "Host::hostIfs()");
    }
    
    for (struct ifaddrs * p = ifs; p != nullptr; p = p->ifa_next) 
        if (p->ifa_addr && p->ifa_addr->sa_family == AF_INET)
            interfaces.push_back(*((struct sockaddr_in *)(p->ifa_addr)));
    
    if (ifs != nullptr) freeifaddrs(ifs);

    return interfaces;
}