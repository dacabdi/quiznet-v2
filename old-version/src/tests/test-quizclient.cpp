#include "Exception.h"
#include "QuizClient.h"
#include "Socket.h"
#include "Host.h"

#include <iostream>
#include <ostream>
#include <istream>
#include <sstream>
#include <sys/wait.h>
#include <chrono>
#include <thread>

void runTest(const std::string& name, bool test(void));

int main(void)
{   
    Host host("localhost", "8080");
    QuizClient client(&host, false);
    client.run();

    return 0; 
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":" << std::flush;
    std::cout << (test() ? "[OK]" : "[FAILED]") << std::flush;
    std::cout << std::endl;
}