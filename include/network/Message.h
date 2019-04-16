#ifndef __MESSAGE__H__
#define __MESSAGE__H__

//#include "ProtoExcept.h"
#include "Body.h"
#include "Common.h"

#include <string>

class Message 
{
    public:

        Message(const std::string&);
        Message(char, const Body);
        Message(void);

        char type(void) const;
        std::string typestr(void) const;
        Body body(void) const;
        
        std::string serialize(void) const;
        static Message deserialize(const std::string&);

        bool operator==(const Message& ref) const;
        bool operator!=(const Message& ref) const;
        friend std::ostream& operator<<(std::ostream& os, const Message& ref);

    protected:
        
        char _type;
        Body _body;

        static char deserializeType(const std::string& str);
};

#endif // __MESSAGE_H__