#ifndef __ITAGCOLLECTION__H__
#define __ITAGCOLLECTION__H__

#include "Tag.h"
#include <vector>

class ITagCollection
{
    public: 
    
        // get tags
        virtual const Tag& at(const size_t idx) const = 0;
        virtual const std::vector<Tag>& getAllTags(void) const = 0;
        
        // accounting
        virtual size_t size(void) const = 0;

        // stringify
        virtual std::string serialize(void) const = 0;

        virtual ~ITagCollection(){};

    protected:

        ITagCollection(){};
};

#endif // __ITAGCOLLECTION__H__