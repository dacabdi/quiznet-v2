#include "Test.h"
#include "ContestServer.h"

#include <thread>

TEST

    CASE("createServer")

        Host host;
        QuizBook qb;
        
        ContestServer server(qb,host);
        server.SetLogger([&](const std::string& str){
            std::cout << "SERVERLOG:" << str << std::endl;
        });
        
    ENDCASE

ENDTEST

RUNTEST