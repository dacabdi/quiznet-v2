#include "UniformRandom.h"
#include "ChoiceCollection.h"
#include <iostream>
#include "Utils.h"

void foo(IChoiceCollection& test)
{
    test.getAllChoices();
}

int main(void)
{
    UniformRandom<size_t> uf;
    
    for(size_t i = 0; i < 100; ++i)
        std::cout << uf.generate(10,20) << " " << std::flush;
    std::cout << std::endl;


    /*std::map<int,int> m;
    m.emplace(1,1);
    m.emplace(2,2);
    m.emplace(3,3);
    m.emplace(4,4);
    m.emplace(5,5);

    std::vector<int> v = utils::getAllKeys<int,int>(m);

    for(int &k : v)
        std::cout << k << " " << std::endl;*/

    return 0; 
}