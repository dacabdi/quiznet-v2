#include "Tag.h"

Tag::Tag(const std::string& tag)
: _tag(tag) {}

const std::string& Tag::getText(void) const
{
    return _tag;
}

std::string Tag::serialize(void) const
{
    return getText();
}

std::ostream& operator<<(std::ostream &os, const Tag& ref)
{
    os << ref.serialize();
    return os;
}

bool Tag::operator==(const Tag& ref) const
{
    return _tag == ref._tag;
}

bool Tag::operator!=(const Tag& ref) const
{
    return !operator==(ref);
}