#ifndef __TCPSOCKET__H__
#define __TCPSOCKET__H__

#include "Except.h"
#include "Host.h"

#include <istream>
#include <ostream>
#include <string>
#include <sstream>
#include <thread> // for timed out accept

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
    #define __DF_BACKLOG 5
#endif

class TcpSocket
{
    public :

        // socket default constructor
        TcpSocket(void);

        // write
        ssize_t write(const std::string&); // from a string
        ssize_t write(std::istream&);      // from an input stream
        
        // read
        ssize_t     read(char *, ssize_t);  // read into sized n buffer
        std::string read(ssize_t);          // read n chars to string
        std::string read(void);             // read everything

        // socket operations (control socket lifecycle)
        void      Connect(const Host&) const;
        void      Bind(const Host&) const;
        void      Bind(void) const; // binds on all ifs on random port
        void      Listen(int = __DF_BACKLOG) const;
        TcpSocket Accept(std::chrono::duration<std::chrono::seconds> timeout = std::chrono::seconds(60)) const;
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
};

#endif // __TCPSOCKET__H__