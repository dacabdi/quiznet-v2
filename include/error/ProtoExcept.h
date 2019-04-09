#ifndef __PROTOEXCEPT__H__
#define __PROTOEXCEPT__H__

#include <stdexcept>
#include <string>
#include <sstream>

#include "Error.h"

class ProtoExcept : public std::runtime_error
{
    protected:
        
        const size_t      _number;
        const std::string _extra;
        const std::string _what;
        std::string       _full;
        
        void init(void)
        {
            std::ostringstream oss;
            ErrorMessage err = Errors.at(_number);

            oss << std::to_string(_number) << std::endl;
            oss << err.symbol << std::endl;
            oss << err.message << std::endl << "." << std::endl;
            oss << _extra << std::endl << "." << std::endl;

            _full = oss.str();
        }

    public:
    
        ProtoExcept(ssize_t number, const std::string& extra = "")
        :   std::runtime_error(""),
            _number(number),
            _extra(extra)
        {
            init();
        };

    	const char * what () const throw ()
        {
            return _full.c_str();
        }
};

#endif // __PROTOEXCEPT__H__