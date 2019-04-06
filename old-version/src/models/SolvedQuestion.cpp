#include "SolvedQuestion.h"

// ------------- CONSTRUCTORS -------------

SolvedQuestion::SolvedQuestion(const std::string& s)
{
    init(s);
}

SolvedQuestion::SolvedQuestion(std::istream& is)
{
    init(is);
}

SolvedQuestion::SolvedQuestion(Question question, char solution)
{
    init(question, solution);
}

// ---------- PUBLIC INTERFACE ------------

char SolvedQuestion::getSolution(void) const
{
    return _solution;
}

const Question& SolvedQuestion::getQuestion(void) const
{
    return _question;
}

std::string SolvedQuestion::serialize(void) const
{
    std::ostringstream oss;
    oss << _question << _solution << std::endl;
    return oss.str();
}

// -------------- PROTECTED METHODS -----------

void SolvedQuestion::init(const std::string& str)
{
    std::istringstream iss(str);
    init(iss);
}

void SolvedQuestion::init(std::istream& is)
{
    Question question(is);
    char solution = deserializeSolution(is);

    init(question, solution);
}

void SolvedQuestion::init(Question question, char solution)
{
    if(!question.getChoices().hasChoice(solution)){ 
        std::string temp = ""; temp.push_back(solution);
        throw Exception("Provided solution " + temp +
                        " does not exist in the question.",
                        "SolvedQuestion::init()", "", false);
    }

    _question = question;
    _solution = solution;
}

char SolvedQuestion::deserializeSolution(std::istream& is) const
{
    std::string solutionLine = "";
    std::getline(is, solutionLine);

    if(solutionLine.length() != 1)
        throw std::invalid_argument(
            std::string("SolvedQuestion::deserializeSolution():")
            + "Length error formatting. Solution line read: " 
            + solutionLine); 
    
    return solutionLine.front();
}

// --------------- OPERATORS -----------------

bool SolvedQuestion::operator==(const SolvedQuestion& ref) const
{
    return (_question == ref._question
        &&  _solution == ref._solution );
}

bool SolvedQuestion::operator!=(const SolvedQuestion& ref) const
{
    return !operator==(ref);
}

std::ostream& operator<<(std::ostream& os, const SolvedQuestion& ref)
{
    os << ref.serialize();
    return os;
}