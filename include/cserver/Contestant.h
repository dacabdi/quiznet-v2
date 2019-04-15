#ifndef __CONTESTANT_H__
#define __CONTESTANT_H__

#include "TcpSocket.h"

#include <string>

class Contestant
{
    public:
    
        Contestant(const std::string nick, TcpSocket socket);

        uint32_t getScore(void) const;
        uint32_t increaseScore(void);

        TcpSocket Conn;
        const std::string Nickname;

    protected:

        uint32_t _score;
};

#endif // __CONTESTANT_H__
