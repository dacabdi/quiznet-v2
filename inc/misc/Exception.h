#ifndef __EXCEPTION__H__
#define __EXCEPTION__H__

// ensures XSI version of strerror_r
#ifdef _GNU_SOURCE
    #undef _GNU_SOURCE
#endif
#include <errno.h>
#include <string.h>

#include "Error.h"

#include <stdexcept>
#include <string>
#include <sstream>

class ProtocolException : public std::runtime_error
{
    protected:
        
        const size_t _number;
        const std::string _extra;
        const std::string _what;
        std::string _full;
        
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
    
        ProtocolException(ssize_t number, const std::string& extra = "")
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

class Exception : public std::runtime_error
{
    public:
        
        const std::string _what;
        const std::string _where;
        const std::string _extraMsg;
        const bool _displayErrno;
        int _errnoInt;
        std::string _errno;
        std::string _errstr;

    protected:    
    
        std::string _full;

        void init(void)
        {
            _full = "\nWhere: " + _where + "\n" 
                  + "What: " + _what + "\n"
                  + "Extra: " + _extraMsg + "\n";

            _errnoInt = errno;
            if(_displayErrno)
            {
                _errno = std::to_string(_errnoInt);
                char buffer[1024];
                bzero(buffer, 1024);
                int r = strerror_r(_errnoInt, buffer, 1024);
                _errstr = (!r ? buffer : "Could not retrieve error string"); 
            
                _full += "Errno: " + _errno + "\n"
                      + "Errstr: " + _errstr + "\n";
            } 
        }

    public:
    
        Exception(const std::string& what,
                  const std::string& where, 
                  const std::string& extraMsg = "", 
                  bool displayErrno = true)
        :   std::runtime_error(""),
            _what(what), 
            _where(where), 
            _extraMsg(extraMsg), 
            _displayErrno(displayErrno) 
        {
            init();
        };

    	const char * what () const throw ()
        {
            return _full.c_str();
        }

        int errNo(void)
        {
            return _errnoInt;
        }
};

#endif //__EXCEPTION__H__