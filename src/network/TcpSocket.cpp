#include "TcpSocket.h"

TcpSocket::TcpSocket(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) throw Except("Error opening socket", "TcpSocket::Socket()");

    // avoid 'address already in use' issue
    int yes=1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) 
        throw Except("Error setting SO_REUSEADDR option", "TcpSocket::TcpSocket()");

    _fd = fd;
}

// move constructor
TcpSocket::TcpSocket(TcpSocket&& other)
: TcpSocket(-1)
{
    swap(*this, other);
}

// assignment operator
TcpSocket &TcpSocket::operator=(TcpSocket&& other)
{
    if(this != &other)
        swap(*this, other);
    
    return *this;
}

void swap(TcpSocket& a, TcpSocket& b) // nothrow
{
    using std::swap; // enable ADL
    swap(a._fd, b._fd);
}

ssize_t TcpSocket::write(const std::string& s)
{
    ssize_t w = send(_fd, s.c_str(), s.size(), 0);

    if (w < (ssize_t)0)
            throw Except("Error writing to socket", "TcpSocket::write()");

    return w;
}

ssize_t TcpSocket::write(std::istream& is)
{
    std::streamsize n = __BUFWR;
    ssize_t t = 0, c = 0; // total, current
    char buff[__BUFWR];

    while(!is.eof())
    {
        is.get(buff, n, '\0');
        c = send(_fd, buff, strlen(buff), 0);

        if (c < (ssize_t)0)
            throw Except("Error writing to socket", "TcpSocket::write()", 
                            "Last buffered content from input stream [" 
                            + std::string(buff) + "]");

        t += c;
    }

    return t;
}

ssize_t TcpSocket::read(char *buff, ssize_t n)
{
    ssize_t r = 0;
    int flags = 0;

    r = recv(_fd, buff, n, flags);

    if (r < 0)
        throw Except("Error reading from socket", "TcpSocket::read()", 
                        "Last buffered content [" + std::string(buff) + "]");

    return r;
}

std::string TcpSocket::read(const ssize_t n)
{
    /*
     * Did not reuse read(char*, ssize_t) because in the event
     * that an exception is thrown, the buffer is not dealloc.
     */

    ssize_t r = 0;
    int flags = 0;
    char *buff = new char[n+1];
    bzero(buff, sizeof(char) * (n+1));

    r = recv(_fd, buff, n, flags);

    if (r < 0)
    {
        std::string extra = "Last buffered content [" + std::string(buff) + "]";
        delete [] buff; throw Except("Error reading from socket", "TcpSocket::read()", extra);
    }

    buff[r] = '\0';
    std::string str(buff);
    delete [] buff;

    return str;
}

std::string TcpSocket::read(void) 
{
    std::string chunk, msg;

    do
    {
        chunk = this->read(__BUFRD);
        msg.append(chunk);
    }
    while(chunk.size() == __BUFRD);

    return msg;
}

void TcpSocket::Connect(const Host& host)
{
    if(!host.count())
       throw Except("Host has no addresses to connect to", "TcpSocket::connect()"); 

    std::string failedNames = "";
    for(size_t i = 0; i < host.count(); ++i)
    {
        const struct sockaddr_in addr = host.address(i);
        if(connect(_fd, (struct sockaddr *)&addr, sizeof(addr)) != -1) 
            return;
        
        failedNames.append(host.fullName(i) + ";");
    }

    throw Except("Failed to connect to '" + failedNames + "'", "TcpSocket::connect()");
}

void TcpSocket::Bind(const Host& host)
{
    const struct sockaddr_in addr = host.address();
    int r = bind(_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (r == -1) throw Except("Failed to bind socket", "TcpSocket::bind()");
}

// passive binding
void TcpSocket::Bind(void)
{   
    Host h(nullptr, "0");
    this->Bind(h);
}

void TcpSocket::Listen(int backlog)
{
    if (listen(_fd, backlog) == -1)
        throw Except("Failed to listen on socket", "TcpSocket::listen()");
}

TcpSocket TcpSocket::Accept(void)
{
    // accept connection
    struct sockaddr_storage address_stg;
    socklen_t address_stg_len = sizeof address_stg;
    int cfd = accept(_fd, (struct sockaddr *)&address_stg, &address_stg_len);

    if (cfd == -1)
       throw Except("Failed to accept connection on socket", "TcpSocket::accept()");

    return TcpSocket(cfd);
}

void TcpSocket::Close(void)
{
    if(_fd == -1) return;
    
    if (close(_fd) != 0)
        throw Except("Failed to close socket", "TcpSocket::close()");

    _fd = -1;
}

void TcpSocket::Shutdown(void)
{
    if (shutdown(_fd, SHUT_RDWR) != 0)
        throw Except("Failed to shutdown socket", "TcpSocket::shutdown()");
}

int TcpSocket::fd(void) const
{
    return _fd;
}

bool TcpSocket::bound(void) const
{
    // if not bound, port will be 0
    return this->local().port(); // TODO find a better convention
}

Host TcpSocket::local(void) const
{
    struct sockaddr_in address;
    socklen_t address_len = sizeof address;

    if(getsockname(_fd, (struct sockaddr *)&address, &address_len))
        throw Except("Failed to retrieve local socket name", "TcpSocket::local()");

    return Host(address);
}

bool TcpSocket::hasPeer(void) const
{
    // TODO find a better design
    try {
        this->peer();
    } catch (const Except& e) {
        if(errno == ENOTCONN) return false;
        else throw e;
    }

    return true;
}

Host TcpSocket::peer(void) const
{
    struct sockaddr_in address;
    socklen_t address_len = sizeof address;

    if(getpeername(_fd, (struct sockaddr *)&address, &address_len))
        throw Except("Failed to retrieve peer's socket name", "TcpSocket::peer()");

    return Host(address);
}

TcpSocket::~TcpSocket()
{
    this->Close();
}

// protected constructor
TcpSocket::TcpSocket(int fd)
{
    _fd = fd;
}