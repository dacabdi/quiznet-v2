#ifndef __SOCKET__H__
#define __SOCKET__H__

#include "Exception.h"
#include "ISocket.h"
#include "IHost.h"
#include "Host.h"
#include "Escape.h"

#include <istream>
#include <ostream>
#include <string>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

class Socket : public ISocket
{
    public :

        Socket(void) : Socket(IPv4, StreamSocket, TCP) {};

        Socket(AddressDomain addressDomain, 
               SocketType socketType, 
               Protocol protocol);

        ~Socket();

        int getDescriptor(void) const override;

        ssize_t writeToSocket(std::istream& is) override;
        ssize_t writeToSocket(const std::string& s) override;

        ssize_t readFromSocket(char *buffer, 
                            ssize_t = __DATA_BUFFER_READ) override;
        ssize_t readFromSocket(std::string& stringBuffer, 
                            ssize_t nbytes = __DATA_BUFFER_READ) override;
        ssize_t readFromSocket(std::ostream& os, 
                           ssize_t nbytes = __DATA_BUFFER_READ) override;
        std::string readFromSocket(void) override;

        AddressDomain getAddressDomain(void) const override;
        Protocol getProtocol(void) const override;
        SocketType getSocketType(void) const override;

        void closeSocket(void) override;
        void shutdownSocket(void) override;
        void bindSocket(IHost& host) override;
        bool isBinded(void) const override;
        const IHost& getBindedHost(void) const override;
        void startListening(int = 5) override;
        void acceptConnection(void) override;
        void connectTo(IHost&) override;
        uint16_t getPort(void) const override;
        std::string getAddress(void) const override;

    protected : 
    
        int _sd;
        AddressDomain _domain;
        SocketType _type;
        Protocol _protocol;
        bool _open = false;
        bool _binded = false;
        const IHost * _bindedTo = nullptr;

        Socket(int);
};

#endif // __SOCKET__H__