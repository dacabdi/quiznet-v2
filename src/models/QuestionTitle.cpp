#include "QuestionTitle.h"

// ----------- CONSTRUCTORS ----------------

QuestionTitle::QuestionTitle(const std::string& title)
{
    _title = deserialize(title);
}

QuestionTitle::QuestionTitle(std::istream& is)
{
    _title = deserialize(is);
}

// ----------- PUBLIC INTERFACE ----------------

const std::string& QuestionTitle::getText(void) const
{
    return _title;
}

std::string QuestionTitle::serialize(void) const
{
    std::ostringstream oss;
    oss << _title << std::endl << "." << std::endl;
    return oss.str();
}

// ---------- OPERATORS -------------------

std::ostream& operator<<(std::ostream &os, const QuestionTitle& question)
{
    os << question.serialize();
    return os;
}

bool QuestionTitle::operator==(const QuestionTitle& ref) const
{
    return _title == ref._title;
}

bool QuestionTitle::operator!=(const QuestionTitle& ref) const
{
    return !operator==(ref);
}

// ----------- PRIVATE METHODS ----------------

std::string QuestionTitle::deserialize(std::istream& is)
{
    std::string title;
    std::string line;

    // if the first line is '.', no question, malformed message? TODO ASK!
    std::getline(is, line);
    if(line == ".")
        throw Except("Failed to deserialize question title", ___WHERE, "Empty question title", false);
    
    title.append(line);
    std::getline(is, line);
    
    while(line != ".")
    {
        title.append("\n");
        title.append(line);
        std::getline(is, line);
    }
    
    return title;
}

std::string QuestionTitle::deserialize(const std::string& str)
{
    size_t pos = str.find("\n.\n");
    if(pos == std::string::npos)
        throw Except("Failed to deserialize question title", ___WHERE, "Title format does not end on '\\n.\\n', provided title: " + str, false);

    return str.substr(0, pos);
}