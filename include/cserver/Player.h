#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "TcpSocket.h"

#include <string>

class Player
{
    public:

        Player(const std::string nick, TcpSocket& conn);

        std::string getNick(void) const;
        TcpSocket& getConnection(void);
        uint32_t getScore(void) const;

        uint32_t increaseScore(void);

    protected:

        std::string _nick;
        TcpSocket& _conn;
        uint32_t _score;
};

#endif // __CONTEST_SESSION_H__
