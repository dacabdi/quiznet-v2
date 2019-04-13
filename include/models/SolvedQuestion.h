#ifndef __SOLVEDQUESTION__H__
#define __SOLVEDQUESTION__H__

#include "Except.h"
#include "Question.h"
#include <string>

class SolvedQuestion
{
    public:

        SolvedQuestion(){};
        SolvedQuestion(const std::string& s);
        SolvedQuestion(std::istream& is);
        SolvedQuestion(Question question, char solution);

        ~SolvedQuestion(){};

        char getSolution(void) const;
        const Question& getQuestion(void) const;
        std::string serialize(void) const;

        bool operator==(const SolvedQuestion& ref) const;
        bool operator!=(const SolvedQuestion& ref) const;
        friend std::ostream& operator<<(std::ostream& os, const SolvedQuestion& ref);

    protected:

        char _solution;
        Question _question;

        void init(const std::string& s);
        void init(std::istream& is);
        void init(Question question, char solution);

        char deserializeSolution(std::istream& is) const;
};

#endif // __SOLVEDQUESTION__H__