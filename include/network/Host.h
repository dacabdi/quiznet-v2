#ifndef __HOST__H__
#define __HOST__H__

#include "Except.h"

#include <string>
#include <ostream>
#include <vector>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>
#include <utility>
#include <ifaddrs.h>

class Host
{
    public:

        // constructors
        Host(const char * const ip, const char * const port);
        Host(const std::string& ip, const std::string& port);
        Host(struct sockaddr_in addr);
        Host(void); // default constructor

        // public interface
        std::string        ip(size_t entry = 0)        const;
        uint16_t           port(size_t entry = 0)      const;
        std::string        fullName(size_t entry = 0)  const;
        struct sockaddr_in address(size_t entry = 0)   const;
        std::string        canonName(void)             const;
        size_t             count(void)                 const;
        
        // helpers
        static std::vector<struct sockaddr_in> hostIfs(void);

    protected:

        std::vector<struct sockaddr_in> _addr;
        std::string _canonname;
};

#endif // __HOST__H__

