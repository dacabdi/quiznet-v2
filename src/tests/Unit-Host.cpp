#include "Host.h"
#include "Test.h"

#include <string>

TEST

    CASE("buildHost_www.google.com:80")
        // setup
        std::string name = "www.google.com", port = "80";
        Host h(name, port);
        // assert
        AssertEqual(h.port(), (uint16_t)std::stoul(port));
        AssertEqual(h.canonName(), name);
        // (no teardown)
    ENDCASE,

    CASE("buildHost_localhost")
        // setup
        std::string ip = "127.0.0.1", 
                    port = "3306", 
                    localhost = "localhost";
        Host h(localhost, port);
        // assert
        AssertEqual(h.ip(), ip);
        AssertEqual(h.port(), (uint16_t)std::stoul(port));
        AssertEqual(h.fullName(), ip + ":" + port);
        AssertEqual(h.canonName(), localhost);
        // (no teardown)
    ENDCASE,
    
    CASE("buildHost_localhostHttp")
        // setup
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host h(localhost, port);
        // assert
        AssertEqual(h.ip(), ip);
        AssertEqual(h.port(), (uint16_t)80);
        AssertEqual(h.fullName(), ip + ":" + "80");
        AssertEqual(h.canonName(), localhost);
        // (no teardown)
    ENDCASE,

    CASE("buildHost_fromSockaddr")
        // setup
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host h(localhost, port);
        Host hc(h.address());
        
        // assert
        AssertEqual(h.ip(), ip);
        AssertEqual(h.port(), (uint16_t)80);
        AssertEqual(h.fullName(), ip + ":" + "80");
        AssertEqual(h.canonName(), localhost);

        AssertEqual(hc.ip(), ip);
        AssertEqual(hc.port(), (uint16_t)80);
        AssertEqual(hc.fullName(), ip + ":" + "80");
        // (no teardown)
    ENDCASE,
    
    CASE("copyConstructorHost")
        // setup
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host h(localhost, port);
        Host hc = h;
        // assert
        AssertEqual(h.ip(), ip);
        AssertEqual(h.port(), (uint16_t)80);
        AssertEqual(h.fullName(), ip + ":" + "80");
        AssertEqual(hc.ip(), ip);
        AssertEqual(hc.port(), (uint16_t)80);
        AssertEqual(hc.fullName(), ip + ":" + "80");
        // (no teardown)
    ENDCASE,

    CASE("copyAssignOperator")
        // setup
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        
        Host h1(ip, port);
        Host h2;
        h2 = h1;

        // assert
        AssertEqual(h2.ip(), ip);
        AssertEqual(h2.port(), (uint16_t)80);
        AssertEqual(h2.fullName(), ip + ":" + "80");
        // (no teardown)
    ENDCASE,

    CASE("moveHost")
        // setup
        std::string ip = "127.0.0.1", port = "http", localhost = "localhost";
        Host temp = Host(localhost, port);
        Host h = std::move(temp);
        // assert
        AssertEqual(h.ip(), ip);
        AssertEqual(h.port(), (uint16_t)80);
        AssertEqual(h.fullName(), ip + ":" + "80");
        // (no teardown)
    ENDCASE,

    CASE("noninitializedHost")
        // setup
        Host h;
        // assert
        AssertExcept(h.ip());
        AssertExcept(h.port());
        AssertExcept(h.fullName());
        AssertExcept(h.address());
        AssertEqual(h.count(), (size_t)0);
        // (no teardown)
    ENDCASE,

    CASE("passiveHostAnyIfAnyPort")
        // setup
        Host h(nullptr, "0");
        
        // assert
        AssertEqual(h.ip(), std::string("0.0.0.0"));
        AssertEqual(std::to_string(h.port()), std::string("0"));
    ENDCASE,

    CASE("passiveHostAnyIfSpecificPort")
        // setup
        Host h(nullptr, "http");
        
        // assert
        AssertEqual(h.ip(), std::string("0.0.0.0"));
        AssertEqual(std::to_string(h.port()), std::string("80"));
    ENDCASE

ENDTEST

RUNTEST