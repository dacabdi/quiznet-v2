#ifndef __TAGCOLLECTION__H__
#define __TAGCOLLECTION__H__

#include "Tag.h"
#include "Utils.h"

#include <vector>
#include <istream>
#include <sstream>

class TagCollection
{
    public: 

        TagCollection(){};
        TagCollection(const std::string& str);
        TagCollection(std::istream& ss);

        // get tags
        const Tag& at(const size_t idx) const;
        const std::vector<Tag>& getAllTags(void) const;
        
        // accounting
        size_t size(void) const;

        // stringify
        std::string serialize(void) const;

        // operators
        bool operator==(const TagCollection& ref) const;
        bool operator!=(const TagCollection& ref) const;
        friend std::ostream& operator<<(std::ostream &os, const TagCollection& ref);

    protected:

        std::vector<Tag> _tags;

        std::vector<Tag> deserialize(const std::string& str);
};

#endif // __TAGCOLLECTION__H__