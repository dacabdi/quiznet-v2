#ifndef __ICHOICECOLLECTION__H__
#define __ICHOICECOLLECTION__H__

#include "Choice.h"
#include <map>

class IChoiceCollection
{
    public: 
    
        // get choices
        virtual const Choice& getChoiceById(const char id) const = 0;
        virtual const std::map<const char, const Choice>&
            getAllChoices(void) const = 0;
        
        virtual bool hasChoice(const char id) const = 0; 

        // accounting
        virtual size_t size(void) const = 0;
        virtual char lastChoiceLetter(void) const = 0; 

        // stringify
        virtual std::string serialize(void) const = 0;
        virtual std::string getText(void) const = 0;

        virtual ~IChoiceCollection(){};

    protected:

        IChoiceCollection(){};
};

#endif // __ICHOICECOLLECTION__H__