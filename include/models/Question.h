#ifndef __QUESTION__H__
#define __QUESTION__H__

#include "Tag.h"
#include "TagCollection.h"
#include "Choice.h"
#include "ChoiceCollection.h"
#include "QuestionTitle.h"

#include <istream>
#include <ostream>
#include <sstream>

class Question
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

        const TagCollection& getTags(void) const;
        const QuestionTitle& getQuestionTitle(void) const;
        const ChoiceCollection& getChoices(void) const;
        std::string serialize(void) const;
        std::string present(void) const;

        friend std::ostream& operator<<(std::ostream &os, const Question& ref);
        bool operator==(const Question& ref) const;
        bool operator!=(const Question& ref) const;

    protected:

        TagCollection _tags;
        QuestionTitle _questionTitle;
        ChoiceCollection _choices;

        void init(std::istream& is);
        void init(TagCollection tags,
                  QuestionTitle questionTitle,
                  ChoiceCollection choices);

};

#endif // __QUESTION__H__