#ifndef __EXCEPT__H__
#define __EXCEPT__H__

#include <clocale>

#include <string.h>
#include <errno.h>

// ensures XSI version of strerror_r
#ifdef __cplusplus
extern "C"
    {
#endif
    extern 
    int __xpg_strerror_r(int errcode,char* buffer,size_t length);
    #define strerror_r __xpg_strerror_r

#ifdef __cplusplus
    }
#endif

#include <stdexcept>
#include <string>

class Except : public std::runtime_error
{
    public:
        
        const std::string _what;
        const std::string _where;
        const std::string _extraMsg;
        const bool        _displayErrno;
        int               _errnoInt;
        std::string       _errno;
        std::string       _errstr;

    protected:
    
        std::string _full;

        void init(void)
        {
            _full = "\nWhere: " + _where    + "\n" 
                  +   "What: "  + _what     + "\n"
                  +   "Extra: " + _extraMsg + "\n";

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
    
        Except(const std::string& what,
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

#endif //__EXCEPT__H__