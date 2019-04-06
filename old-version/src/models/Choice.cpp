#include "Choice.h"

Choice::Choice(const std::string& text)
: _text(text)
{}

std::string Choice::getText(void) const
{
    return _text;
}

std::string Choice::serialize(void) const
{
    return _text;
}

std::ostream& operator<<(std::ostream &os, const Choice& choice)
{
    os << choice.serialize();
    return os;
}

bool Choice::operator==(const Choice& ref) const
{
    return _text == ref._text;
}

bool Choice::operator!=(const Choice& ref) const
{
    return !operator==(ref);
}