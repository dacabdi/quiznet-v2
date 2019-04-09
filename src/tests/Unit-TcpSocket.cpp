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
        bool r = true;
        TcpSocket s;
        s.Bind(Host("localhost", "0"));

        // assert
        r = assertNotEqual(s.fd(), -1) && r;
        r = assertEqual(s.bound(), true) && r;
        r = assertEqual(s.local().ip(), std::string("127.0.0.1")) && r;
        r = assertNotEqual(std::to_string(s.local().port()), std::string("0")) && r;

        return r;
    ENDCASE,

    CASE("socketBindLocalhostPort")
        // setup
        bool r = true;
        TcpSocket s;
        s.Bind(Host("localhost", "6666"));

        // assert
        r = assertNotEqual(s.fd(), -1) && r;
        r = assertEqual(s.bound(), true) && r;
        r = assertEqual(s.local().ip(), std::string("127.0.0.1")) && r;
        r = assertEqual(std::to_string(s.local().port()), std::string("6666")) && r;
        
        return r;
    ENDCASE,

    CASE("socketBindOnAllIfs")
        // setup
        bool r = true;
        TcpSocket s;
        Host h = Host("0.0.0.0", "0");
        s.Bind(h);

        // assert
        r = assertNotEqual(s.fd(), -1) && r;
        r = assertEqual(s.bound(), true) && r;
        
        //r = assertEqual(s.local().ip(), std::string("127.0.0.1")) && r;
        //r = assertEqual(std::to_string(s.local().port()), std::string("6666")) && r;
        
        return r;
    ENDCASE,

    CASE("nonBound_getLocalName")
        // setup
        bool r = true;
        TcpSocket s;

        // assert
        r = assertNotEqual(s.fd(), -1) && r;
        r = assertEqual(s.bound(), false) && r;
        r = assertEqual(s.local().ip(), std::string("0.0.0.0")) && r;
        r = assertEqual(std::to_string(s.local().port()), std::string("0")) && r;
        
        return r;
    ENDCASE,

    CASE("getPeerNonConnected")
        // setup
        bool r = true;
        TcpSocket s;

        // assert
        r = assertNotEqual(s.fd(), -1) && r;
        r = assertEqual(s.hasPeer(), false) && r;
        
        return r;
    ENDCASE,

    CASE("attemptConnectOnBunchOfDeadAddr")
        // setup
        bool r = true;
        TcpSocket s;
        Host ifs(nullptr, "0");

        assertExcept(s.Connect(ifs), r);

        return r;
    ENDCASE,

    CASE("closedTcpSocket")
        // setup
        bool r = true;
        TcpSocket s;
        s.Close();

        // assert
        r = assertEqual(s.fd(), -1) && r;

        assertExcept(s.bound(), r);
        assertExcept(s.local(), r);
        assertExcept(s.hasPeer(), r);
        assertExcept(s.peer(), r);
        
        assertExcept(s.write("dummy"), r);
        assertExcept(s.read(), r);

        assertExcept(s.Bind(), r);
        assertExcept(s.Accept(), r);
        assertExcept(s.Listen(), r);
        assertExcept(s.Connect(Host("dummy","0")), r);
        
        return r;
    ENDCASE

ENDTEST

RUNTEST