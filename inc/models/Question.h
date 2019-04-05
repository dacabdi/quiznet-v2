#ifndef __QUESTION__H__
#define __QUESTION__H__

#include "Tag.h"
#include "TagCollection.h"
#include "Choice.h"
#include "ChoiceCollection.h"
#include "QuestionTitle.h"
#include "IQuestion.h"

#include <istream>
#include <ostream>
#include <sstream>

class Question : public IQuestion
{
    public:

        Question(){};

        // create from a string reading id
        Question(const std::string& str);

        // create from stream reading id
        Question(std::istream& is);
        
        // create from pieces
        Question(TagCollection tags,
                 QuestionTitle question,
                 ChoiceCollection choices);

        ~Question(){};

        const TagCollection& getTags(void) const override;
        const QuestionTitle& getQuestionTitle(void) const override;
        const ChoiceCollection& getChoices(void) const override;
        std::string serialize(void) const override;
        std::string present(void) const override;

        friend std::ostream& operator<<(
            std::ostream &os, const Question& ref);
        bool operator==(const Question& ref) const;
        bool operator!=(const Question& ref) const;

    protected:

        TagCollection _tags;
        QuestionTitle _questionTitle;
        ChoiceCollection _choices;

        void init(std::istream& is);
        void init(TagCollection tags,
                  QuestionTitle questionTitel,
                  ChoiceCollection choices);

};

#endif // __QUESTION__H__