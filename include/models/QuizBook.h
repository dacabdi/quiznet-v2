#ifndef __QUIZBOOK__H__
#define __QUIZBOOK__H__

#include "Except.h"
#include "UniformRandom.h"  // getRandomQuestion
#include "Utils.h"          // getAllKeys
#include "SolvedQuestion.h"

#include <stdexcept>
#include <functional>
#include <istream>

class QuizBook
{
    public:

        QuizBook(void){};
        QuizBook(const std::string& str);
        QuizBook(std::istream& is);
        ~QuizBook(){};

        // obtain question
        const SolvedQuestion& getQuestionById(uint32_t id) const;
        const SolvedQuestion& getRandomQuestion(void) const;
        
        // insert a new question
        uint32_t insertQuestion(const SolvedQuestion question);
        uint32_t insertQuestion(uint32_t id, const SolvedQuestion question);

        // delete a question
        SolvedQuestion deleteQuestionById(const uint32_t id);

        // check if question exists
        bool hasQuestion(const uint32_t id) const;

        // delete all questions
        void clear(void);

        // save and load
        std::ostream& writeTo(std::ostream& os) const;
        std::istream& readFrom(std::istream& is);

        // stringify the entire quizbook
        std::string serialize(void) const;
        std::ostream& serialize(std::ostream& os) const;

        // number of questions
        size_t size(void) const;

        bool operator==(const QuizBook &ref) const;
        bool operator!=(const QuizBook &ref) const;
        friend std::ostream& operator<<(std::ostream& os, const QuizBook& ref);

        // public event handlers (with do nothing stubs)
        std::function<void(const SolvedQuestion&, QuizBook&)> onInsert;
        std::function<void(SolvedQuestion&, QuizBook&)> onDelete;
        std::function<void(QuizBook&)> onClear;

    protected:

        std::map<const uint32_t, const SolvedQuestion> _questions;

        void init(const std::string& str);
        void init(std::istream& is);

        uint32_t findFreeId(const uint32_t start = 0) const;
};

#endif