#include "Question.h"

// --------------- CONSTRUCTORS ------------------

Question::Question(const std::string& str)
{
    std::istringstream iss(str);
    init(iss);
}

Question::Question(std::istream& is)
{
    init(is);
}

Question::Question(TagCollection tags, 
                   QuestionTitle questionTitle, 
                   ChoiceCollection choices)
{
    init(tags, questionTitle, choices);
}

// ------------ PUBLIC INTERFACE -----------

const TagCollection& Question::getTags(void) const 
{
    return _tags;
}

const QuestionTitle& Question::getQuestionTitle(void) const
{
    return _questionTitle;
}

const ChoiceCollection& Question::getChoices(void) const
{
    return _choices;
}

std::string Question::serialize(void) const
{
    std::ostringstream oss;
    oss << _tags << _questionTitle << _choices << std::flush;
    return oss.str();
}

std::string Question::present(void) const
{
    std::ostringstream oss;
    oss << _questionTitle.getText() << std::endl;
    oss << _choices.getText() << std::flush;
    return oss.str();
}

// ------------- OPERATORS --------------------

std::ostream& operator<<(std::ostream &os, const Question& ref)
{
    os << ref.serialize();
    return os;
}

bool Question::operator==(const Question& ref) const
{
    return (
           _tags == ref._tags
        && _questionTitle == ref._questionTitle
        && _choices == ref._choices
    );
}

bool Question::operator!=(const Question& ref) const
{
    return !operator==(ref);
}

// ------------- PRIVATE METHODS --------------

void Question::init(std::istream& is)
{
    TagCollection tags(is);
    QuestionTitle questionTitle(is);
    ChoiceCollection choices(is);

    init(tags, questionTitle, choices);
}

void Question::init(TagCollection tags,
                    QuestionTitle questionTitle,
                    ChoiceCollection choices)
{
    _tags = tags;
    _questionTitle = questionTitle;
    _choices = choices;
}