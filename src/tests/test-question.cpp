#include "Question.h"
#include "Escape.h"
#include <iostream>
#include <ostream>

bool questionProgrammaticallyGoldenCase(void);
bool questionProgrammaticallyTwoChoices(void);
bool questionProgrammaticallyEquality(void);
bool questionProgrammaticallyMultilineTitle(void);

bool questionStringGoldenCase(void);
bool questionStringTwoChoices(void);
bool questionStringEquality(void);
bool questionStringMultilineTitle(void);
bool questionStringEmptyTags(void);

bool questionStringLessThanTwoChoices(void);
bool questionStringNoChoices(void);
bool questionStringUnterminatedQuestionTitle(void);
bool questionStringUnterminatedChoices(void);

void runTest(const std::string& name, bool test(void));

int main(void)
{
    runTest("questionProgrammaticallyGoldenCase", 
        questionProgrammaticallyGoldenCase);
    runTest("questionProgrammaticallyTwoChoices", 
        questionProgrammaticallyTwoChoices);
    runTest("questionProgrammaticallyEquality", 
        questionProgrammaticallyEquality);
    runTest("questionProgrammaticallyMultilineTitle", 
        questionProgrammaticallyMultilineTitle);
    
    runTest("questionStringGoldenCase", 
        questionStringGoldenCase);
    runTest("questionStringTwoChoices",
        questionStringTwoChoices);
    runTest("questionStringEquality",
        questionStringEquality);
    runTest("questionStringMultilineTitle",
        questionStringMultilineTitle);
    runTest("questionStringEmptyTags",
        questionStringEmptyTags);
    runTest("questionStringLessThanTwoChoices",
        questionStringLessThanTwoChoices);
    runTest("questionStringNoChoices",
        questionStringNoChoices);
    runTest("questionStringUnterminatedQuestionTitle",
        questionStringUnterminatedQuestionTitle);
    runTest("questionStringUnterminatedChoices",
        questionStringUnterminatedChoices);

    return 0; 
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":";
    std::cout << (test() ? "[OK]" : "[FAILED]");
    std::cout << std::endl;
}

bool questionProgrammaticallyGoldenCase(void)
{
    std::string expectedSerialization("presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        "(d) Benjamin Franklin\n"
        ".\n"
        ".\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices("(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        "(d) Benjamin Franklin\n"
        ".\n"
        ".\n");

    Question question(tags, questionTitle, choices);

    if(question.getTags() != tags)
        return false;

    if(question.getQuestionTitle() != questionTitle)
        return false;

    if(question.getTags() != tags)
        return false;

    if(question.serialize() != expectedSerialization)
        return false;

    std::ostringstream os;
    os << question;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}

bool questionProgrammaticallyTwoChoices(void)
{
    std::string expectedSerialization(
        "presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    Question question(tags, questionTitle, choices);

    if(question.getTags() != tags)
        return false;

    if(question.getQuestionTitle() != questionTitle)
        return false;

    if(question.getTags() != tags)
        return false;

    if(question.serialize() != expectedSerialization)
        return false;

    std::ostringstream os;
    os << question;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}

bool questionProgrammaticallyEquality(void)
{
    std::string expectedSerialization1(
        "presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    TagCollection tags1("presidents, US history");
    QuestionTitle questionTitle1(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices1(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    std::string expectedSerialization2(
        "network, protocols, application layer\n"
        "Which protocol provides\nguaranteed delivery\n"
        ".\n"
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n");

    TagCollection tags2("network, protocols, application layer");
    QuestionTitle questionTitle2(
        "Which protocol provides\nguaranteed delivery\n"
        ".\n");
    ChoiceCollection choices2(
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n");

    Question question1(tags1, questionTitle1, choices1);
    Question question2(tags2, questionTitle2, choices2);
    Question question3(tags1, questionTitle1, choices1);
    Question question4(tags2, questionTitle2, choices1);
        
    if(question1.getTags() != tags1)
        return false;

    if(question1.getQuestionTitle() != questionTitle1)
        return false;

    if(question1.getTags() != tags1)
        return false;

    if(question1.serialize() != expectedSerialization1)
        return false;

    std::ostringstream os1;
    os1 << question1;
    if(os1.str() != expectedSerialization1)
        return false;

    if(question2.getTags() != tags2)
        return false;

    if(question2.getQuestionTitle() != questionTitle2)
        return false;

    if(question2.getTags() != tags2)
        return false;

    if(question2.serialize() != expectedSerialization2)
        return false;

    std::ostringstream os2;
    os2 << question2;
    if(os2.str() != expectedSerialization2)
        return false;

    if(question1 == question2)
        return false;
    
    if(question1 != question3)
        return false;

    if(question1 == question4)
        return false;

    return true;
}

bool questionProgrammaticallyMultilineTitle(void)
{
    std::string expectedSerialization(
        "presidents, US history\n"
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    Question question(tags, questionTitle, choices);

    if(question.getTags() != tags)
        return false;

    if(question.getQuestionTitle() != questionTitle)
        return false;

    if(question.getTags() != tags)
        return false;

    if(question.serialize() != expectedSerialization)
        return false;

    std::ostringstream os;
    os << question;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}



bool questionStringGoldenCase(void)
{
    std::string s("presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        "(d) Benjamin Franklin\n"
        ".\n"
        ".\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices("(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        "(d) Benjamin Franklin\n"
        ".\n"
        ".\n");

    Question question(s);

    if(question.getTags() != tags)
        return false;

    if(question.getQuestionTitle() != questionTitle)
        return false;

    if(question.getTags() != tags)
        return false;

    if(question.serialize() != s)
        return false;

    std::ostringstream os;
    os << question;
    if(os.str() != s)
        return false;

    return true;
}

bool questionStringTwoChoices(void)
{
    std::string s(
        "presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    Question question(s);

    if(question.getTags() != tags)
        return false;

    if(question.getQuestionTitle() != questionTitle)
        return false;

    if(question.getTags() != tags)
        return false;

    if(question.serialize() != s)
        return false;

    std::ostringstream os;
    os << question;
    if(os.str() != s)
        return false;

    return true;
}

bool questionStringEquality(void)
{
    std::string expectedSerialization1(
        "presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    TagCollection tags1("presidents, US history");
    QuestionTitle questionTitle1(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices1(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    std::string expectedSerialization2(
        "network, protocols, application layer\n"
        "Which protocol provides\nguaranteed delivery\n"
        ".\n"
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n");

    TagCollection tags2("network, protocols, application layer");
    QuestionTitle questionTitle2(
        "Which protocol provides\nguaranteed delivery\n"
        ".\n");
    ChoiceCollection choices2(
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n");

    Question question1(expectedSerialization1);
    Question question2(expectedSerialization2);
    Question question3(expectedSerialization1);

    if(question1.getTags() != tags1)
        return false;

    if(question1.getQuestionTitle() != questionTitle1)
        return false;

    if(question1.getTags() != tags1)
        return false;

    if(question1.serialize() != expectedSerialization1)
        return false;

    std::ostringstream os1;
    os1 << question1;
    if(os1.str() != expectedSerialization1)
        return false;

    if(question2.getTags() != tags2)
        return false;

    if(question2.getQuestionTitle() != questionTitle2)
        return false;

    if(question2.getTags() != tags2)
        return false;

    if(question2.serialize() != expectedSerialization2)
        return false;

    std::ostringstream os2;
    os2 << question2;
    if(os2.str() != expectedSerialization2)
        return false;

    if(question1 != question1)
        return false;

    if(question1 == question2)
        return false;

    if(question1 != question3)
        return false;

    return true;
}

bool questionStringMultilineTitle(void)
{
    std::string expectedSerialization(
        "presidents, US history\n"
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    Question question(expectedSerialization);

    if(question.getTags() != tags)
        return false;

    if(question.getQuestionTitle() != questionTitle)
        return false;

    if(question.getTags() != tags)
        return false;

    if(question.serialize() != expectedSerialization)
        return false;

    std::ostringstream os;
    os << question;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}

bool questionStringEmptyTags(void)
{
    std::string expectedSerialization(
        "\n"
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    TagCollection tags("");
    QuestionTitle questionTitle(
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    Question question(expectedSerialization);

    if(question.getTags() != tags)
        return false;

    if(question.getQuestionTitle() != questionTitle)
        return false;

    if(question.getTags() != tags)
        return false;

    if(question.serialize() != expectedSerialization)
        return false;

    std::ostringstream os;
    os << question;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}


bool questionStringLessThanTwoChoices(void)
{
    std::string input(
        "network, protocols, application layer\n"
        "Which protocol provides\nguaranteed delivery\n"
        ".\n"
        "(a) TCP\n"
        ".\n"
        ".\n");

    try
    {
        Question question(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool questionStringNoChoices(void)
{
    std::string input(
        "network, protocols, application layer\n"
        "Which protocol provides\nguaranteed delivery\n"
        ".\n"
        ".\n"
        ".\n");

    try
    {
        Question question(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool questionStringUnterminatedQuestionTitle(void)
{
    std::string input(
        "network, protocols, application layer\n"
        "Which protocol provides\nguaranteed delivery"
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n");

    try
    {
        Question question(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool questionStringUnterminatedChoices(void)
{
    std::string input(
        "network, protocols, application layer\n"
        "Which protocol provides\nguaranteed delivery\n"
        ".\n"
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
    );

    try
    {
        Question question(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

