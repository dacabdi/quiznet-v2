#include "Contestant.h"

Contestant::Contestant(const std::string nick, TcpSocket conn)
: Conn(std::move(conn)), Nickname(nick), _score(0)
{}

uint32_t Contestant::getScore(void) const
{
    return _score;
}

uint32_t Contestant::increaseScore(void)
{
    return (++_score);
}


