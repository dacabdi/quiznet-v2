#include "ChoiceCollection.h"

// ------------- CONSTRUCTORS ---------------

ChoiceCollection::ChoiceCollection(const std::string& str)
{
    std::istringstream iss(str);
    init(iss);
}

ChoiceCollection::ChoiceCollection(std::istream& is)
{
    init(is);
}

// ------------- PUBLIC INTERFACE ---------------

const Choice& ChoiceCollection::getChoiceById(const char id) const
{
    return _choices.at(id);
}

const std::map<const char, const Choice>& 
    ChoiceCollection::getAllChoices(void) const
{
    return _choices;
}

bool ChoiceCollection::hasChoice(const char id) const
{
    return _choices.find(id) != _choices.end();
}

size_t ChoiceCollection::size(void) const
{
    return _choices.size();
}

char ChoiceCollection::lastChoiceLetter(void) const
{
    // no need to check for empty, it is never empty
    return (--_choices.end())->first;
    
}

std::string ChoiceCollection::serialize(void) const
{
    std::ostringstream oss;

    for (const std::pair<const char, const Choice>& pair : _choices)
    {
        oss << "(" << pair.first << ") ";
        oss << pair.second.serialize() << std::endl;
        oss << "." << std::endl;
    }

    oss << "." << std::endl;

    return oss.str();
}

std::string ChoiceCollection::getText(void) const
{
    std::ostringstream oss;

    for (const std::pair<const char, const Choice>& pair : _choices)
    {
        oss << "(" << pair.first << ") ";
        oss << pair.second.serialize() << std::endl;
    }

    return oss.str();
}

// ------------- PRIVATE METHODS ---------------

void ChoiceCollection::init(std::istream& is)
{
    _choices = deserializeAllChoices(is);
}

std::map<const char, const Choice>
    ChoiceCollection::deserializeAllChoices(std::istream& is) const 
{
    std::map<const char, const Choice> choices;
    bool firstPeriod = false, 
         secondPeriod = false;
    char currentLetter = 'a';
    std::string buffer;
    size_t count = 0;

    while(!secondPeriod)
    {
        std::getline(is, buffer);
        
        if (buffer == "." && !firstPeriod)
            firstPeriod = true;
        else if (buffer == "." && firstPeriod)
            secondPeriod = true;
        else
        {
            firstPeriod = false;

            // validate format and deserialize
            const std::pair<const char, const Choice> pair = 
                deserializeChoice(buffer);

            // check not repeated
            if (choices.find(pair.first) != choices.end())
                throw std::invalid_argument(
                    "ChoiceCollection::deserializeAllChoices():"
                    "Repeated choice letter:" + pair.first);

            if (pair.first != currentLetter) {
                std::string expected = "";
                expected.push_back(currentLetter);
                throw std::invalid_argument(
                     "ChoiceCollection::deserializeAllChoices():"
                     "Choice letter out of order. "
                     "Expected (" + expected +
                     ") and received (" + pair.first + ").");
            }

            // all checks passed
            choices.insert(pair);
            currentLetter++;
            count++;
        }
    }

    if (count < 2)
        throw std::invalid_argument(
            "ChoiceCollection::deserializeAllChoices():"
            "Must have at least 2 choices, Only " + 
            std::to_string(count) + " were provided.");

    return choices;
}

const std::pair<const char, const Choice> 
    ChoiceCollection::deserializeChoice(const std::string& str) const
{
    if(!validateChoice(str))
        throw std::invalid_argument(
            "ChoiceCollection::deserializeAllChoices():"
            "Invalid choice format: [" + str + "]");

    const std::pair<const char, const Choice> 
        pair(str.at(1), Choice(str.substr(4)));

    return pair;
}

bool ChoiceCollection::validateChoice(const std::string& str) const
{
    bool valid = true;

    if  (   str.length() < 4 
        ||  str[0] != '('
        ||  str[1] < 'a'
        ||  str[1] > 'z'
        ||  str[2] != ')'
        ||  str[3] != ' ' 
        )   valid = false;

    return valid;
}

// ------------- OPERATORS ---------------

std::ostream& operator<<(std::ostream &os, const ChoiceCollection& choice)
{
    os << choice.serialize();
    return os;
}

bool ChoiceCollection::operator==(const ChoiceCollection& ref) const
{
    return utils::mapsEqual<const char, const Choice>(
        _choices, 
        ref._choices
    ); 
}

bool ChoiceCollection::operator!=(const ChoiceCollection& ref) const
{
    return !operator==(ref);
}