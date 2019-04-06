#include <iostream>
#include <thread>
#include <functional>

int main(int argc, char *argv[])
{
    std::function<void (int)> f = [](int number){
        std::cout << number << std::endl;
    };

    std::thread th1(f, 1);
    std::thread th2(f, 3);

    th1.join();
    th2.join();

    return 0;
}