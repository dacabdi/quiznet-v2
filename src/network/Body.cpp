#include "Body.h"

Body::Body(const std::string& str)
: _length(str.length()), _content(str)
{}

Body::Body(const char * const cstr)
: Body(std::string(cstr))
{}

Body::Body() 
: Body("")
{}

Body Body::deserialize(const std::string& str)
{
    // expects a length value n followed by and EOL and then n bytes of content
    ssize_t length = 0, pos;
    std::string content = "";

    try {
        pos = str.find('\n');
        length = std::stoul(str.substr(0, pos));
    } catch (const std::exception& e) {
        throw ProtoExcept(MSGDSL, 
            "Failed to deserialize message body [length] field\n"
            "InnerException : " + std::string(e.what()) + "\n" +
            "PassedSerialization : " + str + "\n");
    }

    try {
        content = str.substr(pos+1, length);
    } catch (const std::exception& e) {
        throw ProtoExcept(MSGDSL, 
            "Failed to deserialize message body [content] field\n" 
            "InnerException : " + std::string(e.what()) + "\n" +
            "PassedSerialization : " + str + "\n");
    }

    return Body(content);
}

ssize_t Body::length(void) const
{
    return _length;
}

std::string Body::content(void) const
{
    return _content;
}

std::string Body::serialize(void) const
{
    std::ostringstream oss;
    oss << length() << std::endl;
    oss << content();
    return oss.str();
}

bool Body::operator==(const Body& ref) const
{
    return length() == ref.length() && content() == ref.content();
}

bool Body::operator!=(const Body& ref) const
{
    return !operator==(ref);
}

std::ostream& operator<<(std::ostream& os, const Body& ref)
{
    os << ref.serialize();
    return os;
}