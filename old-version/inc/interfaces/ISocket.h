#ifndef __ISOCKET__H__
#define __ISOCKET__H__

#include "IHost.h"

#include <istream>
#include <ostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <functional>

// extend for other types

enum SocketType {
    StreamSocket = SOCK_STREAM,
    DatagramSocket = SOCK_DGRAM,
    RawSocket = SOCK_RAW 
};

enum AddressDomain {
    IPv4 = AF_INET,
    IPv6 = AF_INET6
};

enum Protocol {
    DefaultProtocol = 0,
    TCP = IPPROTO_TCP,
    UDP = IPPROTO_UDP
};

#ifndef __DATA_BUFFER_READ
    #define __DATA_BUFFER_READ 4096
#endif

#ifndef __DATA_BUFFER_WRITE
    #define __DATA_BUFFER_WRITE 4096
#endif

class ISocket
{
    public:

        virtual int getDescriptor(void) const = 0;
        virtual ssize_t writeToSocket(std::istream&) = 0;
        virtual ssize_t writeToSocket(const std::string&) = 0;
        virtual ssize_t readFromSocket(char *buffer, 
                                       ssize_t = __DATA_BUFFER_READ) = 0;
        virtual ssize_t readFromSocket(std::string& stringBuffer, 
                                ssize_t nbytes = __DATA_BUFFER_READ) = 0;
        virtual ssize_t readFromSocket(std::ostream&, 
                                       ssize_t = __DATA_BUFFER_READ) = 0;
        virtual std::string readFromSocket(void) = 0;
        virtual void closeSocket(void) = 0;
        virtual void shutdownSocket(void) = 0; 
        virtual void bindSocket(IHost&) = 0;
        virtual bool isBound(void) const = 0;
        virtual const IHost& getBoundHost(void) const = 0;
        virtual void startListening(int = 5) = 0;
        virtual void acceptConnection(void) = 0;
        virtual void connectTo(IHost&) = 0;
        virtual uint16_t getPort(void) const = 0;
        virtual std::string getAddress(void) const = 0;
        
        std::function<void(ISocket&, const IHost&, ISocket*)> 
                                        onIncomingConnection;
        std::function<void(ISocket&, const IHost&, ISocket*)> 
                                        onOutgoingConnection;

        virtual ~ISocket(){};

    protected:

            ISocket(){};
};

#endif // __ISOCKET__H__