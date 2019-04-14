#include "Player.h"

Player::Player(void)
: _nick(""), _score(0), _ready(false)
{}

Player::Player(const std::string nick)
: _nick(nick), _score(0), _ready(false)
{}

std::string Player::getNick(void) const
{
    return _nick;
}

void Player::setReady(bool ready)
{
    _ready = ready;
}

bool Player::isReady(void) const
{
    return _ready;
}

uint32_t Player::getScore(void) const
{
    return _score;
}

uint32_t Player::increaseScore(void)
{
    return (++_score);
}


