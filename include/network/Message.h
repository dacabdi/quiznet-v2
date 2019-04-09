#ifndef __MESSAGE__H__
#define __MESSAGE__H__

#include "Except.h"

#include <string>

class Body 
{
    public:

        ssize_t length(void) const;
        std::string content(void) const;

        Body(const std::string& content);
        Body();

    private:
        
        ssize_t _length;
        std::string _content;
};

class Message 
{
    public:

        char type(void) const;
        std::string typeStr(void) const;
        Body body(void) const;

        std::string serialize(void);
        static Message deserialize(std::string);

        Message(char type, Body body);
        Message(const std::string& str);
        Message(void);
    
    protected:
        
        char _type;
        Body _body;

        char readType(const std::string&);
        Body readBody(const std::string&);
};

class Request : public ProtoReqModel
{
    public:

        Request(char type, Body body) : ProtoReqModel(type, body) {};
        Request(char type, const std::string& str) : ProtoReqModel(type, str) {};
        Request(const std::string& str) : ProtoReqModel(str) {};
        Request(){};
};

class Response : public ProtoReqModel
{
    public:

        Response(char type, Body body) : ProtoReqModel(type, body) {};
        Response(char type, const std::string& str) : ProtoReqModel(type, str) {};
        Response(const std::string& str) : ProtoReqModel(str) {};
        Response(){};
};



#endif // __MESSAGE_H__