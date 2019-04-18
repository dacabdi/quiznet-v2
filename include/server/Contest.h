#ifndef __CONTEST_H__
#define __CONTEST_H__

#include "SolvedQuestion.h"
#include "ContestStats.h"
#include "ContestSession.h"

using question_set = std::map<uint32_t, SolvedQuestion>;
using logger = std::function<void (const std::string&)>;

class Contest
{
    public:

        Contest(uint32_t id);

        uint32_t getId(void);

        void addQuestion(uint32_t id, SolvedQuestion sq);
        question_set getQuestions(void) const;
        
        bool hasRun(void) const;
        void setRun(void);
        
        ContestStats getStats(void) const;
        void setStats(ContestStats);
        
        std::string serialize(void) const;
        static Contest deserialize(std::string);
        static Contest deserialize(std::istream&);

        std::string presentHeadline(void) const;
        std::string presentFull(void) const;

    private:
        
        uint32_t          _id;
        bool              _run;
        question_set      _questions;
        ContestStats      _stats;
};

#endif // __CONTEST_H__