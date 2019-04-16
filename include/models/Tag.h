#ifndef __TAG__H__
#define __TAG__H__

#include "Common.h"

#include <string>
#include <ostream>

class Tag
{
    public:
        
        Tag(){};
        Tag(const std::string& tag);

        const std::string& getText(void) const;
        std::string serialize(void) const;

        ~Tag(){};

        friend std::ostream& operator<<(
            std::ostream &os, const Tag& ref);
        bool operator==(const Tag& ref) const;
        bool operator!=(const Tag& ref) const;

    private:

        std::string _tag;
};

#endif // __TAG__H__