#ifndef __CHOICE__H__
#define __CHOICE__H__

#include "IChoice.h"
#include <stdexcept>

class Choice : public IChoice
{
    private:

        std::string _text;

    public:
        
        Choice(){};
        Choice(const std::string& text);

        std::string getText() const override;
        std::string serialize(void) const override;

        friend std::ostream& operator<<(
            std::ostream &os, const Choice& choice);
        bool operator==(const Choice& ref) const;
        bool operator!=(const Choice& ref) const;
};

#endif // __CHOICE__H__