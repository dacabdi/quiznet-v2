#include "SolvedQuestion.h"
#include "Escape.h"
#include <iostream>

bool solvedQuestionProgrammaticallyGoldenCase(void);
bool solvedQuestionProgrammaticallyTwoChoices(void);
bool solvedQuestionProgrammaticallyMultilineTitle(void);
bool solvedQuestionProgrammaticallyEquality(void);
bool solvedQuestionProgrammaticallyUnequal(void);

bool solvedQuestionProgrammaticallySolutionIsNotAChoice(void);

bool solvedQuestionStringGoldenCase(void);
bool solvedQuestionStringTwoChoices(void);

bool solvedQuestionStringSolutionIsNotAChoice(void);



void runTest(const std::string& name, bool test(void));

int main(void)
{
    runTest("solvedQuestionProgrammaticallyGoldenCase", 
        solvedQuestionProgrammaticallyGoldenCase);
    runTest("solvedQuestionProgrammaticallyTwoChoices", 
        solvedQuestionProgrammaticallyTwoChoices);
    runTest("solvedQuestionProgrammaticallyMultilineTitle", 
        solvedQuestionProgrammaticallyMultilineTitle);
    runTest("solvedQuestionProgrammaticallyEquality",
        solvedQuestionProgrammaticallyEquality);
    runTest("solvedQuestionProgrammaticallyUnequal",
        solvedQuestionProgrammaticallyUnequal);

    runTest("solvedQuestionProgrammaticallySolutionIsNotAChoice",
        solvedQuestionProgrammaticallySolutionIsNotAChoice);

    runTest("solvedQuestionStringGoldenCase", 
        solvedQuestionStringGoldenCase);
    runTest("solvedQuestionStringTwoChoices",
        solvedQuestionStringTwoChoices);

    runTest("solvedQuestionStringSolutionIsNotAChoice",
        solvedQuestionStringSolutionIsNotAChoice);

    return 0;
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":";
    std::cout << (test() ? "[OK]" : "[FAILED]");
    std::cout << std::endl;
}

bool solvedQuestionProgrammaticallyGoldenCase(void)
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
        ".\n"
        "c\n");

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

    char solution = 'c';

    Question question(tags, questionTitle, choices);
    
    SolvedQuestion sq(question, solution);

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getQuestion().getQuestionTitle() != questionTitle)
        return false;

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.serialize() != expectedSerialization)
        return false;

    if(sq.getSolution() != solution)
        return false;

    std::ostringstream os;
    os << sq;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}

bool solvedQuestionProgrammaticallyTwoChoices(void)
{
    std::string expectedSerialization(
        "presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
        "b\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    char solution = 'b';

    Question question(tags, questionTitle, choices);
    
    SolvedQuestion sq(question, solution);

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getQuestion().getQuestionTitle() != questionTitle)
        return false;

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.serialize() != expectedSerialization)
        return false;

    if(sq.getSolution() != solution)
        return false;

    std::ostringstream os;
    os << sq;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}


bool solvedQuestionProgrammaticallyMultilineTitle(void)
{
    std::string expectedSerialization(
        "presidents, US history\n"
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
        "a\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    char solution = 'a';

    Question question(tags, questionTitle, choices);
    
    SolvedQuestion sq(question, solution);

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getQuestion().getQuestionTitle() != questionTitle)
        return false;

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getSolution() != solution)
        return false;

    if(sq.serialize() != expectedSerialization)
        return false;

    std::ostringstream os;
    os << sq;
    if(os.str() != expectedSerialization)
        return false;

    return true;
}



bool solvedQuestionStringGoldenCase(void)
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
        ".\n"
        "c\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        "(d) Benjamin Franklin\n"
        ".\n"
        ".\n");
    
    char solution = 'c';

    SolvedQuestion sq(s);

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getQuestion().getQuestionTitle() != questionTitle)
        return false;

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getSolution() != solution)
        return false;

    if(sq.serialize() != s)
        return false;

    std::ostringstream os;
    os << sq;
    if(os.str() != s)
        return false;

    return true;
}

bool solvedQuestionStringTwoChoices(void)
{
    std::string s("presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
        "a\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");
    
    char solution = 'a';

    SolvedQuestion sq(s);

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getQuestion().getQuestionTitle() != questionTitle)
        return false;

    if(sq.getQuestion().getTags() != tags)
        return false;

    if(sq.getSolution() != solution)
        return false;

    if(sq.serialize() != s)
        return false;

    std::ostringstream os;
    os << sq;
    if(os.str() != s)
        return false;

    return true;
}

bool solvedQuestionProgrammaticallySolutionIsNotAChoice(void)
{
    std::string s("presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
        "c\n");

    TagCollection tags("presidents, US history");
    QuestionTitle questionTitle(
        "Which is the first president of the USA\n.\n");
    ChoiceCollection choices(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

    try
    {
        SolvedQuestion sq(s);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool solvedQuestionStringSolutionIsNotAChoice(void)
{
    std::string input(
        "network, protocols, application layer\n"
        "Which protocol provides\nguaranteed delivery\n"
        ".\n"
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n"
        "c\n"
    );

    try
    {
        SolvedQuestion sq(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool solvedQuestionProgrammaticallyEquality(void)
{
    std::string s("presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
        "a\n");
    
    SolvedQuestion sq1(s);
    SolvedQuestion sq2(s);

    if(sq1 != sq2 || sq2 != sq1)
        return false;

    return true;
}

bool solvedQuestionProgrammaticallyUnequal(void)
{
    std::string s1("presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
        "a\n");
    
    std::string s2("presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        ".\n"
        "a\n");
    
    SolvedQuestion sq1(s1);
    SolvedQuestion sq2(s2);

    if(sq1 == sq2 || sq2 == sq1)
        return false;

    return true;
}