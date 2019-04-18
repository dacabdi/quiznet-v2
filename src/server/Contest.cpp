#include "Contest.h"

Contest::Contest(uint32_t id)
: _id(id), _run(false), 
  _questions(questions_set()), 
  _stats(ContestStats())
{}

uint32_t Contest::getId(void)
{
    return _id;
}

void Contest::addQuestion(uint32_t id, SolvedQuestion sq)
{
    _questions.emplace(id, sq);
}

question_set Contest::getQuestions(void) const
{
    return _questions;
}

bool Contest::hasRun(void) const
{
    return _run;
}

void Contest::setRun(void)
{
    _run = true;
}

ContestStats Contest::getStats(void) const
{
    return _stats;
}

void Contest::setStats(ContestStats stats)
{
    _stats = stats;
}

std::string Contest::serialize(void) const
{
    std::ostringstream oss;

    oss << _id << std::endl;
    oss << _questions.size() << std::endl;
    oss << (_run ? "true" : "false") << std::endl;
    
    for(auto &q : _questions)
        oss << q.first << std::endl
            << q.second;
    
    oss << _stats;

    return oss.str();
}

Contest Contest::deserialize(std::istream& iss)
{
    uint32_t id = 0;
    uint32_t number_of_questions = 0;
    std::string run = "false";

    iss >> id;
    iss >> number_of_questions;
    iss >> run;

    Contest contest(id);   
    
    for(uint32_t i = 0; i < number_of_questions; i++)
    {
        uint32_t qid = 0;
        iss >> qid;

        SolvedQuestion sq(iss);
        contest.addQuestion(qid, sq);
    }
    
    if(run == "true")
    {
        contest.setRun();
        ContestStats stats = ContestStats::deserialize(iss);
        contest.setStats(stats);
    }

    return contest;
}

Contest Contest::deserialize(std::string str)
{
    std::istringstream iss(str);
    return Contest::deserialize(iss);
}

std::string Contest::presentHeadline(void) const
{
    std::ostringstream oss;

    oss << _id << "\t" << _questions.size() << " questions(s), ";
    if(hasRun()) {
        oss << "run, " 
            << " average correct: " << _stats.average() << "; "
            << " maximum correct: " << _stats.max();
    } else {
        oss << "not run";
    }
    oss <<  std::endl;

    return oss.str();
}

std::string Contest::presentFull(void) const
{
    std::ostringstream oss;

    oss << presentHeadline();
    
    if(_run) for(auto& q : _questions)
        oss << "\t" << q.first << "\t" << _stats.percentage(q.first) << std::endl;

    return oss.str();
}