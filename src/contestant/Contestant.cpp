#include "Contestant.h"

Contestant::Contestant(void)
: Conn(std::move(TcpSocket(false))), Nickname(""), _score((uint32_t)0)
{}

Contestant::Contestant(const std::string nick, TcpSocket conn)
: Conn(std::move(conn)), Nickname(nick), _score((uint32_t)0)
{}

// move const
Contestant::Contestant(Contestant&& other)
{
    swap(*this, other);
}

// move-assign
Contestant& Contestant::operator=(Contestant&& other) // move assignment
{
    if(this != &other)
        swap(*this, other);

    return *this;
}

// swap function
void swap(Contestant& a, Contestant& b) // nothrow
{
    using std::swap; // enable ADL
    swap(a.Nickname, b.Nickname);
    swap(a.Conn, b.Conn);
}

uint32_t Contestant::getScore(void) const
{
    return _score;
}

uint32_t Contestant::increaseScore(void)
{
    return (++_score);
}