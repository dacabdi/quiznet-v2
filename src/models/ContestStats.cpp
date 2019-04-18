#include "ContestStats.h"

ContestStats::ContestStats(void)
: _set(score_set()), _contestants(0), _highest(0)
{}

ContestStats::ContestStats(score_set set, const uint32_t contestants, const uint32_t highest)
: _set(set), _contestants(contestants), _highest(highest)
{}

ContestStats::ContestStats(const std::string& str)
: ContestStats(ContestStats::deserialize(str))
{}

ContestStats ContestStats::deserialize(const std::string& str)
{
    std::istringstream iss(str);
    return ContestStats::deserialize(iss);
}

ContestStats ContestStats::deserialize(std::istream& iss)
{
    std::string buff;
    
    // data fields
    uint32_t contestants = 0; 
    uint32_t highest = 0;
    size_t questions = 0;
    score_set set;

    // number of contestants
    iss >> buff;
    try { contestants = (uint32_t)std::stoul(buff); }
    catch (const std::exception &e){
        throw Except("Failed to deserialize [contestants] field", ___WHERE, "Inner Exception: " + std::string(e.what()), false);
    }

    // highest score
    iss >> buff;
    try { highest = (uint32_t)std::stoul(buff); }
    catch (const std::exception &e){
        throw Except("Failed to deserialize [highest-score] field", ___WHERE, "Inner Exception: " + std::string(e.what()), false);
    }

    // number of questions
    iss >> buff;
    try { questions = std::stoul(buff); }
    catch (const std::exception &e){
        throw Except("Failed to deserialize [number-of-questions] field", ___WHERE, "Inner Exception: " + std::string(e.what()), false);
    }

    // read each pair of id -> correct
    for (size_t i = 0; i < questions; ++i)
    {
        uint32_t qid = 0;
        uint32_t correct = 0;
        
        // question id
        if(iss.eof()) throw Except("Premature EOF on deserialize [question-line] [id] field", ___WHERE, "Entry index: " + std::to_string(i), false);
        iss >> qid;

        if(iss.eof()) throw Except("Premature EOF on deserialize [question-line] [correct] field", ___WHERE, "Entry index: " + std::to_string(i), false);
        iss >> correct;
        
        set.emplace(qid, correct);
    }

    return ContestStats(set, contestants, highest);
}

std::string ContestStats::serialize(void) const
{
    std::ostringstream oss;

    oss << _contestants << std::endl;
    oss << _highest     << std::endl;
    oss << _set.size()  << std::endl;
    for (const auto &p : _set)
        oss << p.first << " " << p.second << std::endl;

    return oss.str();
}

uint32_t ContestStats::weight(void) const
{
    return _contestants * (uint32_t)_set.size();
}

double ContestStats::average(void) const
{
    uint32_t w = weight();
    
    if(!w) return 0.;

    double t = 0.;
    for (auto &p : _set) 
        t += p.second;
    
    return t / (double)w;
}

uint32_t ContestStats::max(void) const
{
    uint32_t max = 0;
    
    for (auto &p : _set) 
        max = std::max(max, p.second);
    
    return max;
}

uint32_t ContestStats::highest(void) const
{
    return _highest;
}

score_set ContestStats::set(void) const
{
    return _set;
}

uint32_t ContestStats::contestants(void) const
{
    return _contestants;
}

size_t ContestStats::questions(void) const
{
    return _set.size();
}

double ContestStats::percentage(const uint32_t id) const
{
    if(!_set.count(id))
        throw Except("No stats available for question id provided", ___WHERE, "Question id provided: " + std::to_string(id), false);
    
    return _percentage(_set.at(id));
}

percentage_set ContestStats::percentage(void) const
{
    percentage_set percentages;

    for (const auto &p : _set)
        percentages.emplace(p.first, _percentage(p.second));

    return percentages;
}

std::ostream& operator<<(std::ostream &os, const ContestStats& ref)
{
    os << ref.serialize();
    return os;
}

bool ContestStats::operator==(const ContestStats& ref) const
{
    return _contestants == ref._contestants 
        && utils::mapsEqual(_set, ref._set);
}

bool ContestStats::operator!=(const ContestStats& ref) const
{
    return !operator==(ref);
}

double ContestStats::_percentage(const uint32_t correct) const
{
    return !_contestants ? 0. : (double)correct / (double)_contestants;
}