#ifndef __IQUESTION__H__
#define __IQUESTION__H__

#include "Choice.h"
#include "ChoiceCollection.h"
#include "QuestionTitle.h"
#include "Tag.h"
#include "TagCollection.h"

#include <string>

class IQuestion
{
    public:

        virtual const TagCollection& getTags(void) const = 0;
        virtual const QuestionTitle& getQuestionTitle(void) const = 0;
        virtual const ChoiceCollection& getChoices(void) const = 0;                    
        virtual std::string serialize(void) const = 0;
        virtual std::string present(void) const = 0;

        virtual ~IQuestion(){};

    protected:

        IQuestion(){};
};

#endif // __IQUESTION__H__