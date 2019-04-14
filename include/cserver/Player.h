#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "TcpSocket.h"

#include <string>

class Player
{
    public:

        Player();
        Player(const std::string nick);

        std::string getNick(void) const;
        void setReady(bool ready);
        bool isReady(void) const;
        uint32_t getScore(void) const;

        uint32_t increaseScore(void);

    protected:

        std::string _nick;
        uint32_t _score;
        bool _ready;
};

#endif // __CONTEST_SESSION_H__
