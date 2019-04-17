#ifndef __TCPSOCKET__H__
#define __TCPSOCKET__H__

#include "Except.h"
#include "Host.h"
#include "Common.h"

#include <istream>
#include <ostream>
#include <string>
#include <sstream>
#include <mutex>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#ifndef __BUFRD
    #define __BUFRD 4096
#endif

#ifndef __BUFWR
    #define __BUFWR 4096
#endif

#ifndef __DF_BACKLOG
    #define __DF_BACKLOG 10
#endif

class TcpSocket
{
    public :

        TcpSocket(bool init = true); // socket default constructor
        
        // disallow copy and copy assignment operator
        TcpSocket(const TcpSocket&)            = delete;
        TcpSocket& operator=(const TcpSocket&) = delete;
        
        // allow move and move-assign
        TcpSocket(TcpSocket&& other);      // move constructor
        TcpSocket& operator=(TcpSocket&&); // move assignment
        
        // swap function
        friend void swap(TcpSocket& a, TcpSocket& b); // nothrow

        // write
        ssize_t write(const std::string);  // from a string
        ssize_t write(std::istream&);      // from an input stream
        
        // read
        ssize_t     read(char *, ssize_t);  // read into sized n buffer
        std::string read(ssize_t);          // read n chars to string
        std::string read(void);             // read everything

        // socket operations (control socket lifecycle)
        void      Connect(const Host&);
        void      Bind(const Host&);
        void      Bind(void); // binds on all ifs on random port
        void      Listen(int = __DF_BACKLOG);
        TcpSocket Accept(void);
        TcpSocket AcceptTimeout(void);
        void      Close(void);
        void      Shutdown(void);
        // NOTE: ^ capitalized to avoid collision with same name syscalls
        
        // state and getters
        int fd(void) const;
        
        // this end of the socket
        bool bound(void) const;
        Host local(void) const;

        // other end of the socket
        bool hasPeer(void) const;
        Host peer(void)    const;

        // socket destructor
        ~TcpSocket();

    protected : 
    
        int _fd = -1;
        
        TcpSocket(int);
        void _init_socket(void);
};

#endif // __TCPSOCKET__H__