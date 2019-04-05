#ifndef __IQUESTIONTITLE__H__
#define __IQUESTIONTITLE__H__

#include <string>

class IQuestionTitle
{
    public:
        
        virtual const std::string& getText(void) const = 0;
        virtual std::string serialize(void) const = 0;

        virtual ~IQuestionTitle(){};

    protected:

        IQuestionTitle(){};
};

#endif // __IQUESTIONTITLE__H__