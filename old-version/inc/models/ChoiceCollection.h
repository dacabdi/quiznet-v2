#ifndef __CHOICECOLLECTION__H__
#define __CHOICECOLLECTION__H__

#include "IChoiceCollection.h"
#include "Utils.h" // for map comparison
#include <istream>
#include <ostream>
#include <sstream>

class ChoiceCollection : public IChoiceCollection
{
    public: 

        ChoiceCollection(){};
        ChoiceCollection(const std::string& str);
        ChoiceCollection(std::istream& is);
        ~ChoiceCollection(){};

        // get choices
        const Choice& getChoiceById(const char id) const override;
        const std::map<const char, const Choice>& 
            getAllChoices(void) const override;

        bool hasChoice(const char id) const override;

        // accounting
        size_t size(void) const override;
        char lastChoiceLetter(void) const override; 

        // stringify
        std::string serialize(void) const override;
        std::string getText(void) const override;

        // operators
        bool operator==(const ChoiceCollection& ref) const;
        bool operator!=(const ChoiceCollection& ref) const;
        friend std::ostream& operator<<(
            std::ostream &os, const ChoiceCollection& ref);


    protected:

        // data

        std::map<const char, const Choice> _choices;

        // methods

        void init(std::istream& is);

        std::map<const char, const Choice>
            deserializeAllChoices(std::istream& is) const;

        const std::pair<const char, const Choice> 
            deserializeChoice(const std::string& str) const;

        bool validateChoice(const std::string& str) const;
};

#endif // __CHOICECOLLECTION__H__