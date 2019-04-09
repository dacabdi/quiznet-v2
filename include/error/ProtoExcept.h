#ifndef __PROTOEXCEPT__H__
#define __PROTOEXCEPT__H__

#include "ProtoError.h"

#include <stdexcept>
#include <string>
#include <sstream>

class ProtoExcept
{
    public:
    
        ProtoExcept(ssize_t, const std::string& = "");
        ProtoExcept(const ProtoError&);

        const char * what () const throw ();

        const ProtoError Error;

    protected:

        std::string _what;
};

#endif // __PROTOEXCEPT__H__