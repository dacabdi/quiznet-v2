#include "Player.h"

Player::Player(const std::string nick, TcpSocket& conn)
: _nick(nick), _conn(conn), _score(0)
{}

std::string Player::getNick(void) const
{
    return _nick;
}

TcpSocket& Player::getConnection(void)
{
    return _conn;
}

uint32_t Player::getScore(void) const
{
    return _score;
}

uint32_t Player::increaseScore(void)
{
    return (++_score);
}