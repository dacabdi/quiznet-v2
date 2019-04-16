#include "Message.h"

// MESSAGE

Message::Message() : _type('\0'), _body(Body()) {}

Message::Message(char type, const Body body)
: _type(type), _body(body) {}

Message::Message(const std::string& str)
: _type(Message::deserializeType(str)), 
  _body(Body::deserialize(str.substr(2)))
{}

std::string Message::serialize(void) const
{
    std::ostringstream oss;
    oss << type() << " ";
    oss << body() << std::endl;
    return oss.str();
}

Message Message::deserialize(const std::string& str)
{
    return Message(str);
}
    
char Message::type(void) const
{
    return _type;
}

std::string Message::typestr(void) const
{
    std::string buff = "";
    buff.push_back(type());
    return buff;
}

Body Message::body(void) const
{
    return _body;
}

bool Message::operator==(const Message& ref) const
{
    return type() == ref.type() && body() == ref.body();
}

bool Message::operator!=(const Message& ref) const
{
    return !operator==(ref);
}

std::ostream& operator<<(std::ostream &os, const Message& ref)
{
    os << ref.serialize();
    return os;
}

char Message::deserializeType(const std::string& str)
{
    try {
        char t = str.at(0);
        
        if(str.at(1) != ' ') 
            throw Except("Message type is not followed by whitespace", ___WHERE, "", false);
        if(!isalpha(t) || !islower(t))
            throw Except("Message type is not lowercase alpha", ___WHERE, "", false);
        return t;
    } catch (const std::exception& e) {
        throw ProtoExcept(MSGDSL, 
            "Failed to deserialize message's [type] field\n" 
            "InnerException: " + std::string(e.what()) + "\n" +
            "PassedSerialization: " + str + "\n" +
            "Where:" + ___WHERE);
    }
}