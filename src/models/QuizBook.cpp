#include "QuizBook.h"

QuizBook::QuizBook(const std::string& str)
{
    init(str);
}

QuizBook::QuizBook(std::istream& is)
{
    init(is);
}

const SolvedQuestion& QuizBook::getQuestionById(uint32_t id) const
{
    if(!hasQuestion(id)) 
        throw Except("Question id " + std::to_string(id) + " does not exist.", ___WHERE, "", false);

    return _questions.at(id);
}

const SolvedQuestion& QuizBook::getRandomQuestion(void) const
{
    if(!size())
        throw Except("Empty Quizbook!", ___WHERE, "", false);

    // create a vector with all the keys
    std::vector<uint32_t> keys;
    for(const std::pair<const uint32_t, const SolvedQuestion>& pair 
    : _questions) keys.push_back(pair.first);

    // select an index of the vector randomly and use that key
    UniformRandom<uint32_t> uf(0, (uint32_t)(keys.size() - 1));
    uint32_t keyId = uf.generate();
    uint32_t id = keys[keyId];

    return getQuestionById(id);
}

uint32_t QuizBook::insertQuestion(const SolvedQuestion question)
{
    // find a free id for the insertion
    uint32_t id = findFreeId();
    return insertQuestion(id, question);
}

uint32_t QuizBook::insertQuestion(uint32_t id, const SolvedQuestion question)
{
    // insert the question with the provided id
    std::pair<std::map<const uint32_t, const SolvedQuestion>::iterator, 
        bool> placement =_questions.emplace(id, question);

    // if insertion did not happen (possibly duplicate id) ...
    if(!placement.second) // ... then except
        throw Except("Attempted to insert duplicate id.", ___WHERE, "Id provided: " + std::to_string(id) + "Question provided (escaped): " + question.serialize(), false);
    
    // if completed then call event handler 
    // with reference to constructed question
    if(onInsert) onInsert((*placement.first).second, *this);

    return id; 
}   

SolvedQuestion QuizBook::deleteQuestionById(const uint32_t id)
{
    // make a copy for the event handler
    SolvedQuestion deletedQuestion = getQuestionById(id);
    
    // delete and check whether the deletion happened
    if(!_questions.erase(id))
        throw Except("Question id " + std::to_string(id) + " does not exist.", ___WHERE, "", false);

    // call onDelete handler with a copy of the question
    if(onDelete) onDelete(deletedQuestion, *this);

    return deletedQuestion;
}

bool QuizBook::hasQuestion(const uint32_t id) const
{
    return (_questions.find(id) != _questions.end());
}

void QuizBook::clear(void)
{
    if(onClear) onClear(*this);
    _questions.clear();
}

std::ostream& QuizBook::writeTo(std::ostream& os) const
{
    return serialize(os);
}

std::istream& QuizBook::readFrom(std::istream& is)
{   
    std::string line;

    while(is >> line)
    {
        uint32_t id = (uint32_t)std::stoul(line); is.get(); // get \n
        SolvedQuestion sq(is);
        
        _questions.emplace(id, sq);
    }

    return is;
}

// stringify the entire quizbook
std::string QuizBook::serialize(void) const
{
    std::ostringstream oss;
    serialize(oss);
    return oss.str();
}

// put the entire quizbook into an output buffer
std::ostream& QuizBook::serialize(std::ostream& os) const
{
    std::map<const uint32_t, const SolvedQuestion>::const_iterator it =
    _questions.begin();

    if(it != _questions.end())
    {
        os << it->first << std::endl << it->second;
        while((++it) != _questions.end())
            os << std::endl
               << it->first
               << std::endl
               << it->second;
    }

    return os;
}

// number of questions
size_t QuizBook::size(void) const
{
    return _questions.size();
}

// ------------- PROTECTED METHODS ----------------

void QuizBook::init(const std::string& str)
{
    std::istringstream ss(str);
    init(ss);
}

void QuizBook::init(std::istream& is)
{
    readFrom(is);
}

uint32_t QuizBook::findFreeId(const uint32_t start) const
{
    for(uint32_t id = start; id < std::numeric_limits<uint32_t>::max(); ++id)
        if(!hasQuestion(id))
            return id;

    throw Except("QuizBook maxed out", ___WHERE, "", false);
}

// ----------------OPERATORS--------------------------

bool QuizBook::operator==(const QuizBook& ref) const
{
    std::map<const uint32_t, const SolvedQuestion>::const_iterator i1, i2;
    
    for(// init 
        i1 = (*this)._questions.begin(), 
        i2 = ref._questions.begin();
        // condition
            i1 != this->_questions.end() && i2 != ref._questions.end()
        &&  i1->first  == i2->first      && i1->second == i2->second;
        // maintenance
        ++i1, ++i2);

    // the iteration ended before hitting the end
    return (i1 != this->_questions.end() || i2 != ref._questions.end());
}

bool QuizBook::operator!=(const QuizBook& ref) const
{
    return !operator==(ref);
}

std::ostream& operator<<(std::ostream& os, const QuizBook& ref)
{
    os << ref.serialize();
    return os;
}