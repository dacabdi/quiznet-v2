#include "Choice.h"
#include "ChoiceCollection.h"
#include <iostream>
#include <ostream>

bool choiceGoldenCase(void);
bool choiceEqualsOperator(void);
bool choiceCollectionGoldenCaseString(void);
bool choiceCollectionGoldenCaseTwoChoicesString(void);
bool choiceCollectionEqualsOperator(void);
bool choiceCollectionNotEnoughChoices(void);
bool choiceCollectionNoChoices(void);
bool choiceCollectionChoicesOutOfOrder(void);
bool choiceCollectionChoicesNotAStarting(void);
bool choiceCollectionMissingOneTerminationPeriods(void);
bool choiceCollectionMissingAllTerminationPeriods(void);
bool choiceCollectionChoiceIdMalformed1(void);
bool choiceCollectionChoiceIdMalformed2(void);
bool choiceCollectionChoiceIdMalformed3(void);
bool choiceCollectionChoiceIdMalformed4(void);

void runTest(const std::string& name, bool test(void));

int main(void)
{
    runTest("choiceGoldenCase", choiceGoldenCase);
    runTest("choiceEqualsOperator", choiceEqualsOperator);
    
    runTest("choiceCollectionGoldenCaseString", 
        choiceCollectionGoldenCaseString);
    runTest("choiceCollectionGoldenCaseTwoChoicesString", 
        choiceCollectionGoldenCaseTwoChoicesString);
    runTest("choiceCollectionEqualsOperator", 
        choiceCollectionEqualsOperator);    
    runTest("choiceCollectionNotEnoughChoices", 
        choiceCollectionNotEnoughChoices);
    runTest("choiceCollectionNoChoices", 
        choiceCollectionNoChoices);
    runTest("choiceCollectionChoicesOutOfOrder", 
        choiceCollectionChoicesOutOfOrder);
    runTest("choiceCollectionChoicesNotAStarting", 
        choiceCollectionChoicesNotAStarting);
    runTest("choiceCollectionMissingOneTerminationPeriods", 
        choiceCollectionMissingOneTerminationPeriods);
    runTest("choiceCollectionChoiceIdMalformed1", 
        choiceCollectionChoiceIdMalformed1);
    runTest("choiceCollectionChoiceIdMalformed2", 
        choiceCollectionChoiceIdMalformed2);
    runTest("choiceCollectionChoiceIdMalformed3", 
        choiceCollectionChoiceIdMalformed3);
    runTest("choiceCollectionChoiceIdMalformed4", 
        choiceCollectionChoiceIdMalformed4);

    return 0; 
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":";
    std::cout << (test() ? "[OK]" : "[FAILED]");
    std::cout << std::endl;
}

bool choiceGoldenCase(void)
{
    std::string s = "This is a choice";
    Choice choice(s);

    if(choice.getText() != s)
        return false;

    if(choice.serialize() != s)
        return false;

    std::ostringstream oss;
    oss << choice;
    if(oss.str() != s)
        return false; 

    return true;
}

bool choiceEqualsOperator(void)
{
    Choice choice1("Choice 1 and 2 are equal");
    Choice choice2("Choice 1 and 2 are equal");
    Choice choice3("Choice 1 and 3 are not equal");

    if(choice1 != choice2) return false;
    if(choice2 != choice1) return false;
    if(choice1 == choice3) return false;
    if(choice2 == choice3) return false;

    return true;
}

bool choiceCollectionGoldenCaseString(void)
{
    std::string input(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        "(d) Benjamin Franklin\n"
        ".\n"
        ".\n"
    );

    ChoiceCollection cc(input);

    if(cc.getChoiceById('a') != Choice("Thomas Jefferson"))
        return false;

    if(cc.getChoiceById('b') != Choice("Abraham Lincoln"))
        return false;

    if(cc.getChoiceById('c') != Choice("George Washington"))
        return false;

    if(cc.getChoiceById('d') != Choice("Benjamin Franklin"))
        return false;

    // skipping getAllChoices... TODO

    if(cc.size() != 4)
        return false;

    if(cc.lastChoiceLetter() != 'd')
        return false;

    if(cc.serialize() != input)
        return false;

    std::ostringstream oss;
    oss << cc;
    if(oss.str() != input)
        return false;

    return true;
}

bool choiceCollectionGoldenCaseTwoChoicesString(void)
{
    std::string input(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    ChoiceCollection cc(input);

    if(cc.getChoiceById('a') != Choice("Thomas Jefferson"))
        return false;

    if(cc.getChoiceById('b') != Choice("Abraham Lincoln"))
        return false;

    // skipping getAllChoices... TODO

    if(cc.size() != 2)
        return false;

    if(cc.lastChoiceLetter() != 'b')
        return false;

    if(cc.serialize() != input)
        return false;

    std::ostringstream oss;
    oss << cc;
    if(oss.str() != input)
        return false;

    return true;
}

bool choiceCollectionEqualsOperator(void)
{
    std::string input1(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    std::string input2(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) Ben Franklim\n"
        ".\n"
        ".\n"
    );

    std::string input3(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Ben Franklim\n"
        ".\n"
        "(c) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    ChoiceCollection cc1(input1);
    ChoiceCollection cc2(input2);
    ChoiceCollection cc3(input3);
    ChoiceCollection cc4(input1);

    if(cc1 == cc2 || cc2 == cc1)
        return false;
    
    if(cc1 != cc4 || cc4 != cc1)
        return false;

    if(cc2 == cc3 || cc3 == cc2)
        return false;

    if(cc3 == cc4 || cc4 == cc3)
        return false;

    return true;
}

bool choiceCollectionNotEnoughChoices(void)
{
    std::string input(
        "(a) Thomas Jefferson\n"
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionNoChoices(void)
{
    std::string input(
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionChoicesOutOfOrder(void)
{
    std::string input(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(c) Abraham Lincoln\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionChoicesNotAStarting(void)
{
    std::string input(
        "(b) Thomas Jefferson\n"
        ".\n"
        "(c) Abraham Lincoln\n"
        ".\n"
        "(d) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionMissingOneTerminationPeriods(void)
{
    std::string input(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionMissingAllTerminationPeriods(void)
{
    std::string input(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionChoiceIdMalformed1(void)
{
    std::string input(
        "(%) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionChoiceIdMalformed2(void)
{
    std::string input(
        "() Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionChoiceIdMalformed3(void)
{
    std::string input(
        "(a% Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}

bool choiceCollectionChoiceIdMalformed4(void)
{
    std::string input(
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b)Abraham Lincoln\n"
        ".\n"
        ".\n"
    );

    try
    {
        ChoiceCollection cc(input);
        return false;
    }
    catch(const std::exception& e)
    {}

    return true;
}
