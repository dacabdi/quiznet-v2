#include "Body.h"
#include "Test.h"

TEST

    CASE("deserializeBody")
        // setup
        std::string str = "8\ntesting\n";
        Body b = Body::deserialize(str);

        // assert
        AssertEqual(b.length(), (ssize_t)8);
        AssertEqual(b.content(), std::string("testing\n"));
        AssertEqual(b.serialize(), str);
    ENDCASE,

    CASE("deserializeBodyWithoutLength_except")
        // setup
        std::string str = "testing\n";
        
        // assert
        AssertExcept(Body::deserialize(str));
    ENDCASE,

    CASE("deserializeBodyLengthLTContent_except")
        // setup
        std::string str = "2\ntesting\n";
        Body b = Body::deserialize(str);
        
        // assert
        AssertEqual(b.length(), (ssize_t)2);
        AssertEqual(b.content(), std::string("te"));
        AssertEqual(b.serialize(), std::string("2\nte"));
    ENDCASE,

    CASE("deserializeBody")
        // setup
        std::string str = "7\ntesting";
        Body b = Body::deserialize(str);

        // assert
        AssertEqual(b.length(), (ssize_t)7);
        AssertEqual(b.content(), std::string("testing"));
        AssertEqual(b.serialize(), str);
    ENDCASE,

    CASE("constructorFromString")
        // setup
        std::string str = "7\ntesting";
        Body b1 = Body::deserialize(str);
        Body b2("testing");

        // assert
        AssertEqual(b1, b2);
    ENDCASE,

    CASE("defaultConstructor")
        // setup
        Body b1;

        // assert
        AssertEqual(b1.content(), std::string(""));
        AssertEqual(b1.length(), (ssize_t)0);
        AssertEqual(b1.serialize(), std::string("0\n"));
    ENDCASE

ENDTEST

RUNTEST