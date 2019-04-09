#ifndef __BODY__H__
#define __BODY__H__

#include "ProtoExcept.h"

#include <string>  // std::string
#include <ostream> // std::ostream
#include <sstream> // std::ostringstream

class Body 
{
    public:

        Body(const std::string&);
        Body(const char * const);
        Body(void);

        static Body deserialize(const std::string& str);

        ssize_t length(void) const;
        std::string content(void) const;
        std::string serialize(void) const;

        bool operator==(const Body&) const;
        bool operator!=(const Body&) const;
        friend std::ostream& operator<<(std::ostream&, const Body&);

    private:
        
        ssize_t _length;
        std::string _content;
};

#endif // __BODY_H__