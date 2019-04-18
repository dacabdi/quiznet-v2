#ifndef __CONTESTANT_H__
#define __CONTESTANT_H__

#include "TcpSocket.h"
#include "Common.h"

#include <string>

class Contestant
{
    public:
    
        Contestant(void);
        Contestant(const std::string nick, TcpSocket socket);

        // disallow copy and copy assignment operator
        Contestant(const Contestant&)            = delete;
        Contestant& operator=(const Contestant&) = delete;
        
        // allow move and move-assign
        Contestant(Contestant&& other);      // move constructor
        Contestant& operator=(Contestant&&); // move assignment
        
        // swap function
        friend void swap(Contestant& a, Contestant& b); // nothrow

        uint32_t getScore(void) const;
        uint32_t increaseScore(void);

        TcpSocket Conn;
        std::string Nickname;

    protected:

        uint32_t _score = (uint32_t)0;
};

#endif // __CONTESTANT_H__