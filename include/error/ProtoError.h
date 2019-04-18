#ifndef __PROTOERROR_H__
#define __PROTOERROR_H__

#include "Except.h"
#include "Common.h"

#include <map>
#include <string>
#include <sstream>

#define UNKERR  0
#define NOTFND  1
#define MALQUE  2
#define CHNFND  3 
#define UNKREQ  4 
#define INVQID  5
#define EMPTYQ  6
#define REQTSZ  7
#define USEDID  8
#define CTNFND  9
#define LATEJN 10
#define FAILGT 11
#define MSGDSL 12
#define NCKDUP 13
#define EMPTYN 14

class ProtoError
{
    public :

        ProtoError(size_t number, const std::string& symbol, const std::string& msg, const std::string& extra);
        ProtoError(size_t number, const std::string& extra); // uses the __err__ map
        ProtoError(const std::string&);
        ProtoError(void);

        size_t      number(void) const;
        std::string symbol(void) const;
        std::string msg(void) const;
        std::string extra(void) const;

        static ProtoError deserialize(const std::string&);
        std::string serialize(void) const;
        std::string present(void) const;

        bool operator==(const ProtoError& ref) const;
        bool operator!=(const ProtoError& ref) const;
        friend std::ostream& operator<<(std::ostream& os, const ProtoError& ref);

        const static std::map<size_t, ProtoError> Types;

    protected:

        size_t      _number;
        std::string _symbol;
        std::string _msg;
        std::string _extra;
};

#endif // __PROTOERROR_H__