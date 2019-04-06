#include "QuestionTitle.h"
#include <iostream>
#include <ostream>

bool questionTitleGoldenCase(void);
bool questionTitleMultilineGoldenCase(void);
bool questionTitleEquality(void);
bool questionTitleMissingTermination(void);
bool questionTitleEmpty(void);

void runTest(const std::string& name, bool test(void));

int main(void)
{
    runTest("questionTitleGoldenCase", questionTitleGoldenCase);
    runTest("questionTitleMultilineGoldenCase", 
        questionTitleMultilineGoldenCase);
    runTest("questionTitleEquality", questionTitleEquality);
    runTest("questionTitleMissingTermination", 
        questionTitleMissingTermination);
    runTest("questionTitleEmpty", questionTitleEmpty);


    return 0; 
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":";
    std::cout << (test() ? "[OK]" : "[FAILED]");
    std::cout << std::endl;
}

#include "Escape.h"

bool questionTitleGoldenCase(void)
{
    std::string s("This is a single line title\n.\n");
    QuestionTitle qt(s);

    if(qt.getText() != "This is a single line title")
        return false;
        
    if(qt.serialize() != s)
        return false;

    std::ostringstream ss;
    ss << qt;
    if(ss.str() != s)
        return false;

    return true;
}

bool questionTitleMultilineGoldenCase(void)
{
    std::string s("A\nmultine\nquestion.\nLook, it has periods!\n.\n");
    QuestionTitle qt(s);

    if(qt.getText() != "A\nmultine\nquestion.\nLook, it has periods!")
        return false;

    if(qt.serialize() != s)
        return false;

    std::ostringstream ss;
    ss << qt;
    if(ss.str() != s)
        return false;

    return true;
}

bool questionTitleEquality(void)
{
    std::string s1("A\nmultine\nquestion.\nLook, it has periods!\n.\n");
    std::string s2("A single line question!\n.\n");
    
    QuestionTitle qt1(s1);
    QuestionTitle qt2(s2);
    QuestionTitle qt3(s1);

    if(qt1 == qt2 || qt2 == qt1)
        return false;

    if(qt1 != qt3 || qt3 != qt1)
        return false;

    return true;
}

bool questionTitleMissingTermination(void)
{
    std::string s1("A\nmultine\nquestion.\nLook, it has periods!\n.");
    std::string s2("A\nmultine\nquestion.\nLook, it has periods!");
    std::string s3("A\nmultine\nquestion.\nLook, it has periods!.\n");

    try
    {
        QuestionTitle cc(s1);
        return false;
    }
    catch(const std::exception& e)
    {}

    try
    {
        QuestionTitle cc(s2);
        return false;
    }
    catch(const std::exception& e)
    {}

    try
    {
        QuestionTitle cc(s3);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool questionTitleEmpty(void)
{
    std::string s1(".\n");
    std::string s2(".\ntesting");

    try
    {
        QuestionTitle cc(s1);
        return false;
    }
    catch(const std::exception& e)
    {}

    try
    {
        QuestionTitle cc(s2);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}