#ifndef __CHOICE__H__
#define __CHOICE__H__

#include "Common.h"
#include <stdexcept>

class Choice
{
    private:

        std::string _text;

    public:
        
        Choice(){};
        Choice(const std::string& text);

        std::string getText() const;
        std::string serialize(void) const;

        friend std::ostream& operator<<(std::ostream &os, const Choice& choice);
        bool operator==(const Choice& ref) const;
        bool operator!=(const Choice& ref) const;
};

#endif // __CHOICE__H__