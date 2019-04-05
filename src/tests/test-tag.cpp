#include "Tag.h"
#include "TagCollection.h"
#include <iostream>
#include <ostream>

bool tagGoldenCase(void);
bool tagCollectionGoldenCase(void);

void runTest(const std::string& name, bool test(void));

int main(void)
{
    runTest("tagGoldenCase", tagGoldenCase);
    runTest("tagCollectionGoldenCase", tagCollectionGoldenCase);

    return 0; 
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":";
    std::cout << (test() ? "[OK]" : "[FAILED]");
    std::cout << std::endl;
}

bool tagGoldenCase(void)
{
    std::string str = "presidents";
    Tag t(str);
    
    if(t.getText() != str)
        return false;

    if(t.serialize() != str)
        return false;

    return true;
}

bool tagCollectionGoldenCase(void)
{
    std::string input = "presidents,US History, politics";

    TagCollection tc(input);
    
    if(tc.at(0).getText() != "presidents")
        return false;
    
    if(tc.at(1).getText() != "US History")
        return false;
    
    if(tc.at(2).getText() != " politics")
        return false;

    if(tc.size() != 3)
        return false;

    if(tc.serialize() != (input + "\n"))
        return false;

    return true;
}
