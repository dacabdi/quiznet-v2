#include "Request.h"

Body::Body(const std::string content)
: _length(content.length()), _content(content) {}

ssize_t Body::getLength(void) const
{
    return _length;
}

std::string Body::getContent(void) const
{
    return _content;
}

ProtoReqModel::ProtoReqModel(char type, Body body)
: _type(type), _body(body) {}

ProtoReqModel::ProtoReqModel(char type, 
const std::string& str)
{
    _type = type;
    _body = Body(str);
}

ProtoReqModel::ProtoReqModel(const std::string& str)
{
    _type = readType(str);
    _body = readBody(str);
}

std::string ProtoReqModel::serialize(void)
{
    std::ostringstream oss;
    oss << _type << " " << _body.getLength() << std::endl;
    oss << _body.getContent();
    oss << std::endl;
    return oss.str();
}

char ProtoReqModel::readType(const std::string& str)
{
    char type;
    try {
        type = str.at(0);
    } catch (const std::exception& e) {
        throw ProtocolException(UNKERR, 
            "Failed to read request type : " + std::string(e.what()));
    }
    
    return type;
}

Body ProtoReqModel::readBody(const std::string& str)
{
    ssize_t length = 0;
    ssize_t pos;
    try {
        pos = str.find('\n');
        length = std::stoul(str.substr(2, pos));
    } catch (const std::exception& e) {
        throw ProtocolException(UNKERR, 
            "Failed to read [length] field" + std::string(e.what()));
    }
    
    std::string content = "";
    try {
        content = str.substr(pos+1, length);
    } catch (const std::exception& e) {
        throw ProtocolException(UNKERR, 
            "Failed to read body contents : " + std::string(e.what()));
    }

    return Body(content);
}

char ProtoReqModel::getType(void) const
{
    return _type;
}

std::string ProtoReqModel::getTypeAsString(void) const
{
    std::string buff = "";
    buff.push_back(getType());
    return buff;
}

Body ProtoReqModel::getBody(void) const
{
    return _body;
}

