#ifndef __EXCEPT__H__
#define __EXCEPT__H__

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

#include "Common.h"

class Except : public std::runtime_error
{
    public:

        Except(const std::string& what, const std::string& where, 
               const std::string& extra = "", bool includeErrstr = true);
        
        Except(void);

        const char * what () const throw ();

        const std::string What;
        const std::string Where;
        const std::string Extra;
        const bool        IncludeErrstr;
        const size_t      Errno;
    
    protected:
    
        // init protected constructor
        Except(const std::string& what, const std::string& where, 
               const std::string& extra, bool includeErrstr,
               const size_t errnumber);

        std::string _what;

        virtual std::string compose(void) const;
};

#endif //__EXCEPT__H__