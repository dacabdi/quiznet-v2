#ifndef __REQUEST__H__
#define __REQUEST__H__

#include "Exception.h"

#include <string>

class Body 
{
    private:
        
        ssize_t _length;
        std::string _content;

    public:

        ssize_t getLength(void) const;
        std::string getContent(void) const;

        Body(const std::string content);
        Body():_length(0), _content("") {};
};

class ProtoReqModel 
{
    protected:
        
        char _type;
        Body _body;

        char readType(const std::string&);
        Body readBody(const std::string&);

    public:

        char getType(void) const;
        std::string getTypeAsString(void) const;
        Body getBody(void) const;

        std::string serialize(void);

        ProtoReqModel(char type, Body body);
        ProtoReqModel(char type, const std::string& str);
        ProtoReqModel(const std::string& str);
        ProtoReqModel(){};
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



#endif