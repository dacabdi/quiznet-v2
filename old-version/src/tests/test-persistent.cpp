#include "Exception.h"
#include "QuizClient.h"
#include "QuizServer.h"
#include "Host.h"
#include "QuizBook.h"
#include <iostream>
#include <ostream>
#include <istream>
#include <sstream>
#include <sys/wait.h>
#include <chrono>
#include <thread>

void runTest(const std::string& name, bool test(void));

bool basicInteraction(void);

int main(void)
{
    runTest("basicInteraction", basicInteraction);

    return 0; 
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":" << std::flush;
    std::cout << (test() ? "[OK]" : "[FAILED]") << std::flush;
    std::cout << std::endl;
}

bool basicInteraction(void)
{
    bool result = true;

    // create pipe for process coordination
    int p[2]; // p[0] < reads, p[1] < writes
    int r = pipe(p); 

    std::string clientScript(
        "p\n"
        "presidents, US history\n"
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
        "c\n"
        "g 0\n"
        "r\n"
        "c\n"
        "r\n"
        "d\n"
        "d 0\n"
        "k\n"
    );

    std::istringstream _clientInput(clientScript);

    if(r < 0)
        throw Exception("Failed to create pipes", "testCommunication()");
    
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process (client)

        // wait for server ready message
        char buffer[6];
        int r = read(p[0], buffer, 5);
        buffer[5] = '\0';

        if(r < 0)
            throw Exception(
                "Child process (client) failed to " 
                "read pipe for ready to accept message", 
                "testCommunication()");

        // if server ready, sleep for some milliseconds, and then connect
        if(std::string(buffer) == "ready")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            // setup client
            Host host("localhost", "8080");
            QuizClient client(&host, true, _clientInput);
            client.run();
        }
        else
        {
            exit(EXIT_FAILURE);
            result = false;
        }

        // if everything went as expected, tell server's process
        std::string resultMsg = (result ? "ok" : "failed");
        std::cout << "client'sresult: " << resultMsg;
        r = write(p[1], resultMsg.c_str(), resultMsg.length());
        if(r < 0)
            throw Exception(
                "Child process (client) failed to"
                " write to pipe final status message", 
                "testCommunication()");

        exit(EXIT_SUCCESS);
    }
    else if (pid > 0)
    {
        // server process (parent proc)

        QuizBook qb;
        Host host("8080");
        Socket socket(IPv4, StreamSocket, TCP);
        std::string quizbookFilename = "test-persistent-db.data";
        QuizServer server(&qb, &host, &socket, quizbookFilename);
        /*server.setLogger([&](const std::string& str){
            std::cout << str << std::flush;
        });*/

        // tell client process we are ready
        char ready[] = "ready";
        int r = write(p[1], ready, strlen(ready));
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to write to pipe", 
                "testCommunication()");
        
        // run server
        server.run();

        // ask the client how it went for him
        char finalStatus[10];
        r = read(p[0], finalStatus, 10);
        finalStatus[r] = '\0';

        if(r < 0)
            throw Exception(
                "Parent process (server) failed to read from pipe"
                " final status message",
                "testCommunication()");

        // if it went well for client also, hurray!
        if(std::string(finalStatus) != "ok")
            result = false;

        std::cout << "client'sresult(on server): " << finalStatus;

        wait(NULL); // wait for the other process to return
    }
    else
    {
        // fork failed
        throw Exception(
            "Fork failed on communication test", 
            "testCommunication()");
    }

    return result;
}