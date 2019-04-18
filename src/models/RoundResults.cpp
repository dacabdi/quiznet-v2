#include "RoundResults.h"

RoundResults::RoundResults()
: RoundResults(false, 0., (uint32_t)0, (uint32_t)0, (uint32_t)0)
{}

RoundResults::RoundResults(const std::string& str)
: RoundResults(deserialize(str))
{}

RoundResults::RoundResults(bool correct, double ratio, uint32_t questions, uint32_t score, uint32_t max)
: _correct(correct), _ratio(ratio), _questions(questions), _score(score), _max(max)
{}

RoundResults RoundResults::deserialize(const std::string& str)
{
    std::istringstream iss(str);
    std::string buff;

    bool correct = false;
    double ratio = 0.;
    uint32_t questions = 0,
             score = 0,
             max = 0;

    std::getline(iss, buff);
    if      (buff == "correct"  ) correct = true;
    else if (buff == "incorrect") correct = false;
    else throw Except("Failed to deserialize [result] string field", ___WHERE, buff, false);

    std::getline(iss, buff);
    try { ratio = std::stod(buff); }
    catch (const std::exception& e){ throw Except("Failed to deserialize [ratio] field", ___WHERE, "String read: " + buff + "\nInner Exception: " + e.what(), false); }

    std::getline(iss, buff);
    try { questions = (uint32_t)std::stoul(buff); }
    catch (const std::exception& e){ throw Except("Failed to deserialize [questions] field", ___WHERE, "String read: " + buff  + "\nInner Exception: " + e.what(), false); }

    std::getline(iss, buff);
    try { score = (uint32_t)std::stoul(buff); }
    catch (const std::exception& e){ throw Except("Failed to deserialize [score] field", ___WHERE, "String read: " + buff + "\nInner Exception: " + e.what(), false); }

    std::getline(iss, buff);
    try { max = (uint32_t)std::stoul(buff); }
    catch (const std::exception& e){ throw Except("Failed to deserialize [max] field", ___WHERE, "String read: " + buff + "\nInner Exception: " + e.what(), false); }

    return std::move(RoundResults(correct, ratio, questions, score, max));
}

std::string RoundResults::serialize(void) const
{
    std::ostringstream oss;
    
    oss << (correct() ? "correct" : "incorrect") << std::endl;
    oss << std::to_string(ratio()) << std::endl;
    oss << std::to_string(questions()) << std::endl;
    oss << std::to_string(score()) << std::endl;
    oss << std::to_string(max()) << std::endl;
    
    return oss.str();
}

bool RoundResults::correct(void) const
{
    return _correct;
}

double RoundResults::ratio(void) const
{
    return _ratio;
}

uint32_t RoundResults::questions(void) const
{
    return _questions;
}

uint32_t RoundResults::score(void) const
{
    return _score;
}

uint32_t RoundResults::max(void) const
{
    return _max;
}

std::string RoundResults::present(void) const
{
    std::ostringstream oss;

    oss << (correct() ? "Correct" : "Incorrect") << ". "
        << (ratio() * 100)
        << "% of contestants answered this question correctly." << std::endl
        << "Your score is " << score() << "/" << questions() << ". "
        << "The top score is currently " << max() << "/" << questions() << "." << std::endl;

    return oss.str();
}

std::ostream& operator<<(std::ostream &os, const RoundResults& ref)
{
    os << ref.serialize();
    return os;
}

bool RoundResults::operator==(const RoundResults& ref) const
{
    return _correct   == ref.correct()
        && _ratio     == ref.ratio()
        && _questions == ref.questions()
        && _score     == ref.score()
        && _max       == ref.max();
}

bool RoundResults::operator!=(const RoundResults& ref) const
{
    return !operator==(ref);
}