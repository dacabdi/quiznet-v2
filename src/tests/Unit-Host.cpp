#include "Host.h"
#include "Test.h"

#include <string>

TEST

    CASE("buildHost_www.google.com:80")
        // setup
        bool r = true;
        std::string name = "www.google.com", port = "80";
        Host h(name, port);
        // assert
        r = assertEqual(h.port(), (uint16_t)std::stoul(port)) && r;
        r = assertEqual(h.canonName(), name) && r;
        // (no teardown)
        return r;
    ENDCASE,

    CASE("buildHost_localhost")
        // setup
        bool r = true;
        std::string ip = "127.0.0.1", 
                    port = "3306", 
                    localhost = "localhost";
        Host h(localhost, port);
        // assert
        r = assertEqual(h.ip(), ip) && r;
        r = assertEqual(h.port(), (uint16_t)std::stoul(port)) && r;
        r = assertEqual(h.fullName(), ip + ":" + port) && r;
        r = assertEqual(h.canonName(), localhost);
        // (no teardown)
        return r;
    ENDCASE,
    
    CASE("buildHost_localhostHttp")
        // setup
        bool r = true;
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host h(localhost, port);
        // assert
        r = assertEqual(h.ip(), ip) && r;
        r = assertEqual(h.port(), (uint16_t)80) && r;
        r = assertEqual(h.fullName(), ip + ":" + "80") && r;
        r = assertEqual(h.canonName(), localhost);
        // (no teardown)
        return r;
    ENDCASE,

    CASE("buildHost_fromSockaddr")
        // setup
        bool r = true;
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host h(localhost, port);
        Host hc(h.address());
        
        // assert
        r = assertEqual(h.ip(), ip) && r;
        r = assertEqual(h.port(), (uint16_t)80) && r;
        r = assertEqual(h.fullName(), ip + ":" + "80") && r;
        r = assertEqual(h.canonName(), localhost);

        r = assertEqual(hc.ip(), ip) && r;
        r = assertEqual(hc.port(), (uint16_t)80) && r;
        r = assertEqual(hc.fullName(), ip + ":" + "80") && r;
        // (no teardown)
        return r;
    ENDCASE,
    
    CASE("copyConstructorHost")
        // setup
        bool r = true;
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host h(localhost, port);
        Host hc = h;
        // assert
        r = assertEqual(h.ip(), ip) && r;
        r = assertEqual(h.port(), (uint16_t)80) && r;
        r = assertEqual(h.fullName(), ip + ":" + "80") && r;
        r = assertEqual(hc.ip(), ip) && r;
        r = assertEqual(hc.port(), (uint16_t)80) && r;
        r = assertEqual(hc.fullName(), ip + ":" + "80") && r;
        // (no teardown)
        return r;
    ENDCASE,

    CASE("copyAssignOperator")
        // setup
        bool r = true;
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        
        Host h1(ip, port);
        Host h2;
        h2 = h1;

        // assert
        r = assertEqual(h2.ip(), ip) && r;
        r = assertEqual(h2.port(), (uint16_t)80) && r;
        r = assertEqual(h2.fullName(), ip + ":" + "80") && r;
        // (no teardown)
        return r;
    ENDCASE,

    CASE("moveHost")
        // setup
        bool r = true;
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host temp = Host(localhost, port);
        Host h = std::move(temp);
        // assert
        r = assertEqual(h.ip(), ip) && r;
        r = assertEqual(h.port(), (uint16_t)80) && r;
        r = assertEqual(h.fullName(), ip + ":" + "80") && r;
        // (no teardown)
        return r;
    ENDCASE,

    CASE("noninitializedHost")
        // setup
        bool r = true;
        Host h;
        // assert
        assertExcept(h.ip(), r);
        assertExcept(h.port(), r);
        assertExcept(h.fullName(), r);
        assertExcept(h.address(), r);
        r = assertEqual(h.count(), (size_t)0) && r;
        // (no teardown)
        return r;
    ENDCASE,

    CASE("passiveHostAnyIfAnyPort")
        // setup
        bool r = true;
        Host h(nullptr, "0");
        
        // assert
        r = assertEqual(h.ip(), std::string("0.0.0.0")) && r;
        r = assertEqual(std::to_string(h.port()), std::string("0")) && r;

        return r;
    ENDCASE,

    CASE("passiveHostAnyIfSpecificPort")
        // setup
        bool r = true;
        Host h(nullptr, "http");
        
        // assert
        r = assertEqual(h.ip(), std::string("0.0.0.0")) && r;
        r = assertEqual(std::to_string(h.port()), std::string("80")) && r;

        return r;
    ENDCASE

ENDTEST

RUNTEST