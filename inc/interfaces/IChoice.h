#ifndef __ICHOICE__H__
#define __ICHOICE__H__

#include <string>
#include <ostream>

class IChoice
{
    public:
        
        virtual std::string getText() const = 0;
        virtual std::string serialize(void) const = 0;

        virtual ~IChoice(){};

    protected:

        IChoice(){};
};

#endif // __ICHOICE__H__