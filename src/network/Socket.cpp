#include "Socket.h"

Socket::Socket(int sd)
{   
    _open = true;
    _sd = sd;
}

Socket::Socket(AddressDomain domain, SocketType type, Protocol protocol)
: _domain(domain), _type(type), _protocol(protocol)
{
    int sd = socket(domain, type, protocol);
    
    if (sd < 0) 
        throw Exception("Error opening socket", "Socket::Socket:()");

    // avoid address already in use
    int yes=1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) 
        throw Exception(
            "Error setting setting SO_REUSEADDR to 1", 
            "Socket::Socket:()");

    _open = true;
    _sd = sd;
}

Socket::~Socket()
{
    if(_open) closeSocket();
}

int Socket::getDescriptor(void) const
{
    return _sd;
}

ssize_t Socket::writeToSocket(std::istream& is)
{
    std::streamsize ssize = __DATA_BUFFER_WRITE;
    ssize_t total = 0, current = 0;

    char buffer[__DATA_BUFFER_WRITE];
    bzero(buffer, __DATA_BUFFER_WRITE);

    while(!is.eof())
    {
        is.get(buffer, ssize, '\0');
        current = strlen(buffer);
        current = write(_sd, buffer, current);

        if (current < (ssize_t)0) // check if last reading was error (-1)
            throw Exception("Error writing to socket", 
                            "Socket::writeToSocket()", 
                            "Last buffered content ["
                            + std::string(buffer) + "]");

        total += current; // uncount the termination char
    }

    return total;
}

ssize_t Socket::writeToSocket(const std::string& s)
{
    std::istringstream iss(s);
    return writeToSocket(iss);
}

ssize_t Socket::readFromSocket(char *buffer, ssize_t nbytes)
{
    ssize_t r = 0;
    int flags = 0;

    r = recv(_sd, buffer, nbytes, flags);

    if (r < 0) // check if last reading was error (-1)
    {
        delete [] buffer; // dealloc before excepting
        throw Exception("Error reading from socket", 
                        "Socket::readFromSocket()", 
                        "Last buffered content ["
                        + std::string(buffer) + "]");
    }

    return r;
}

ssize_t Socket::readFromSocket(std::string& stringBuffer, ssize_t nbytes)
{
    // allocate buffer with an extra char for termination
    char * buffer = new char[nbytes+1];
    bzero(buffer, nbytes+1);
    ssize_t read = 0;
    ssize_t totalRead = 0;
    stringBuffer = "";

    while((read = readFromSocket(buffer, nbytes)) == nbytes)
    {
        totalRead += read;
        buffer[nbytes] = '\0';
        stringBuffer.append(buffer);
        bzero(buffer, nbytes+1);
    }

    totalRead += read;
    buffer[nbytes] = '\0';
    stringBuffer.append(buffer);

    delete [] buffer;

    return read;
}

std::string Socket::readFromSocket(void) 
{
    std::string buffer = "";
    readFromSocket(buffer);
    return buffer;
}

ssize_t Socket::readFromSocket(std::ostream& os, ssize_t nbytes)
{
    std::string stringBuffer = "";
    ssize_t read = readFromSocket(stringBuffer, nbytes);
    os << stringBuffer;

    return read;
}

AddressDomain Socket::getAddressDomain(void) const
{
    return _domain;
}

SocketType Socket::getSocketType(void) const
{
    return _type;
}

Protocol Socket::getProtocol(void) const
{
    return _protocol;
}

void Socket::closeSocket(void)
{
    if (close(_sd) != 0)
        throw Exception("Failed to close socket", "Socket::close()");

    _open = false;
    _binded = false;
    _bindedTo = nullptr;
}

void Socket::shutdownSocket(void)
{
    if (shutdown(_sd, SHUT_RDWR) != 0)
        throw Exception("Error shutting down socket", 
                        "Socket::shutdown()");
}

void Socket::bindSocket(IHost& host)
{
    struct addrinfo *res = host.getAddressInfo();
    int r = bind(_sd, res->ai_addr, res->ai_addrlen);
    if (r == -1) throw Exception("Failed to bind socket",
                                 "Socket::bindSocket()");
    _binded = true;
    _bindedTo = &host;
}

bool Socket::isBinded(void) const
{
    return _binded;
}

const IHost& Socket::getBindedHost(void) const
{
    return *_bindedTo;
}

void Socket::startListening(int backlog)
{
    if (listen(_sd, backlog) == -1)
        throw Exception("Failed to listen on socket",
                        "Socket::startListening()");
}

void Socket::acceptConnection(void)
{
    struct sockaddr_storage peerAddress;
    socklen_t peerAddressSize = sizeof peerAddress;

    int newSocket = accept(_sd, (struct sockaddr *)&peerAddress, 
                            &peerAddressSize);

    if (newSocket == -1)
       throw Exception("Failed to accept connection on socket",
                       "Socket::acceptConnection()");

    // wrapping the descriptor around will cause
    // the socket to be automatically closed
    // when going out of the scope of this method
    Socket connSocket(newSocket);
    struct sockaddr *temp = ((struct sockaddr *)&peerAddress);
    Host host(temp);

    if(onIncomingConnection)
        onIncomingConnection(connSocket, host, this);
}

void Socket::connectTo(IHost& host)
{
    struct addrinfo* res = host.getAddressInfo();
    int c = connect(_sd, res->ai_addr, res->ai_addrlen);

    if (c == -1)
       throw Exception("Failed to connect to host " + host.getAddress(),
                       "Socket::connectTo()");

    if(onOutgoingConnection)
        onOutgoingConnection(*this, host, this);
}

uint16_t Socket::getPort(void) const
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(_sd, (struct sockaddr *)&sin, &len) == -1)
        throw Exception("Couldn't obtain socket name.",
                        "Socket::getPort()");
    
    return ntohs(sin.sin_port);
}

std::string Socket::getAddress(void) const
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(_sd, (struct sockaddr *)&sin, &len) == -1)
        throw Exception("Couldn't obtain socket name.",
                        "Socket::getPort()");
    
    return inet_ntoa(sin.sin_addr);
}