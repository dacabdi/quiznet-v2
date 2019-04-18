#include "Test.h"
#include "ContestServer.h"
#include "ContestmeisterClient.h"

#include <thread>

/*

p 3
presidents, US history
Which is the first president of the USA
.
(a) Thomas Jefferson
.
(b) Abraham Lincoln
.
(c) George Washington
.
(d) Benjamin Franklin
.
.
c
p 6
network, protocols,layered
Which of these protocols guarantee delivery"
.
(a) TCP
.
(b) UDP
.
.
a
p 2
network, protocols,layered
Which of these protocols does 
not provide flow control"
.
(a) TCP
.
(b) UDP
.
.
b

*/


TEST

    CASE("createServer")

        /*Host host("localhost", "3360");
        QuizBook qb;

        std::string clientscript1 = "q\n";
        std::string clientscript2 = "k\n";
        
        ContestServer server(qb,host);
        server.SetLogger([&](const std::string& str){
            std::cout << str << std::endl;
        });

        std::thread server_thread([&]{
            server.run();
        });
        
        std::thread client_1_thread([&]{
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ContestmeisterClient client(Host("localhost", "3360"));
            client.run();
        });
        
        std::thread client_2_thread([&]{
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ContestmeisterClient client(Host("localhost", "3360"));
            client.run();
        });*/

        /*if(client_1_thread.joinable()) client_1_thread.join();
        if(client_2_thread.joinable()) client_2_thread.join();*/
        //if(server_thread.joinable()) server_thread.join();

    ENDCASE

ENDTEST

RUNTEST