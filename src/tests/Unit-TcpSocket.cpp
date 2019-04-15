#include "TcpSocket.h"
#include "Test.h"
#include "UniformRandom.h"

#include <string>
#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

TEST

    CASE("socketBindLocalhostAnyPort")
        // setup
        
        TcpSocket s;
        s.Bind(Host("localhost", "0"));

        // assert
        AssertNotEqual(s.fd(), -1);
        AssertEqual(s.bound(), true);
        AssertEqual(s.local().ip(), std::string("127.0.0.1"));
        AssertNotEqual(std::to_string(s.local().port()), std::string("0"));

        
    ENDCASE,

    CASE("socketBindLocalhostPort")
        // setup
        
        TcpSocket s;
        s.Bind(Host("localhost", "6666"));

        // assert
        AssertNotEqual(s.fd(), -1);
        AssertEqual(s.bound(), true);
        AssertEqual(s.local().ip(), std::string("127.0.0.1"));
        AssertEqual(std::to_string(s.local().port()), std::string("6666"));
        
        
    ENDCASE,

    CASE("socketBindOnAllIfs")
        // setup
        
        TcpSocket s;
        Host h = Host("0.0.0.0", "0");
        s.Bind(h);

        // assert
        AssertNotEqual(s.fd(), -1);
        AssertEqual(s.bound(), true);
        
        //AssertEqual(s.local().ip(), std::string("127.0.0.1"));
        //AssertEqual(std::to_string(s.local().port()), std::string("6666"));
        
        
    ENDCASE,

    CASE("nonBound_getLocalName")
        // setup
        
        TcpSocket s;

        // assert
        AssertNotEqual(s.fd(), -1);
        AssertEqual(s.bound(), false);
        AssertEqual(s.local().ip(), std::string("0.0.0.0"));
        AssertEqual(std::to_string(s.local().port()), std::string("0"));
        
        
    ENDCASE,

    CASE("getPeerNonConnected")
        // setup
        
        TcpSocket s;

        // assert
        AssertNotEqual(s.fd(), -1);
        AssertEqual(s.hasPeer(), false);
        
        
    ENDCASE,

    CASE("attemptConnectOnBunchOfDeadAddr")
        // setup
        
        TcpSocket s;
        Host ifs(nullptr, "0");

        AssertExcept(s.Connect(ifs));

        
    ENDCASE,

    CASE("closedTcpSocket")
        // setup
        
        TcpSocket s;
        s.Close();

        // assert
        AssertEqual(s.fd(), -1);

        AssertExcept(s.bound());
        AssertExcept(s.local());
        AssertExcept(s.hasPeer());
        AssertExcept(s.peer());
        
        AssertExcept(s.write("dummy"));
        AssertExcept(s.read());

        AssertExcept(s.Bind());
        AssertExcept(s.Accept());
        AssertExcept(s.Listen());
        AssertExcept(s.Connect(Host("dummy","0")));
        
        
    ENDCASE,

    CASE("shutdownBlockedAcceptingSocket")
        
        // setup
        TcpSocket s;
        s.Bind();
        s.Listen();

        std::mutex m;

        std::thread window_signal([&](){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            s.Shutdown();
        });

        AssertExcept(s.Accept()); // the exception is expected
        
        window_signal.join();

    ENDCASE,

    CASE("shutdownBlockedAcceptingSocketAfterIncoming")
        
        // setup
        TcpSocket s;
        s.Bind();
        s.Listen();
        Host h = s.local();

        std::mutex m;

        std::thread window_signal([&](){
            std::this_thread::sleep_for(std::chrono::seconds(2));
            s.Shutdown();
        });

        std::thread incoming_conn([&](){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            TcpSocket conn;
            conn.Connect(h);
        });

        s.Accept(); // the exception is expected

        incoming_conn.join();
        window_signal.join();

    ENDCASE,

    CASE("moveConstructorSocket")
        // setup
        TcpSocket s(TcpSocket());

        // no exception will be thrown trying to close the 
        // socket twice...

    ENDCASE,

    CASE("moveAssignOperatorSocket")
        // setup
        TcpSocket s1;
        int s1_fd = s1.fd();

        TcpSocket s2;
        int s2_fd = s2.fd();

        s1 = std::move(s2);

        // assert
        AssertEqual(s1.fd(), s2_fd);
        AssertEqual(s2.fd(), s1_fd);

    ENDCASE,

    CASE("moveConstructorSocketObj")
        // setup
        TcpSocket s1;
        int s1_fd = s1.fd();
        TcpSocket s2(std::move(s1));

        // assert
        AssertEqual(s1.fd(), -1);
        AssertEqual(s2.fd(), s1_fd);

    ENDCASE

ENDTEST

RUNTEST