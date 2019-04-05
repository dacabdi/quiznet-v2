#ifndef __ITAG__H__
#define __ITAG__H__

#include <string>

class ITag
{
    public:
        
        virtual const std::string& getText(void) const = 0;
        virtual std::string serialize(void) const = 0;

        virtual ~ITag(){};
        ITag(){};
};

#endif // __ITAG__H__