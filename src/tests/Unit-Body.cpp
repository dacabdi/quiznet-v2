#include "Body.h"
#include "Test.h"

TEST

    CASE("deserializeBody")
        // setup
        bool r = true;
        std::string str = "8\ntesting\n";
        Body b = Body::deserialize(str);

        // assert
        r = assertEqual(b.length(), (ssize_t)8) && r;
        r = assertEqual(b.content(), std::string("testing\n")) && r;
        r = assertEqual(b.serialize(), str) && r;

        return r;
    ENDCASE,

    CASE("deserializeBodyWithoutLength_except")
        // setup
        bool r = true;
        std::string str = "testing\n";
        
        // assert
        assertExcept(Body::deserialize(str), r);

        return r;
    ENDCASE,

    CASE("deserializeBodyLengthLTContent_except")
        // setup
        bool r = true;
        std::string str = "2\ntesting\n";
        Body b = Body::deserialize(str);
        
        // assert
        r = assertEqual(b.length(), (ssize_t)2) && r;
        r = assertEqual(b.content(), std::string("te")) && r;
        r = assertEqual(b.serialize(), std::string("2\nte")) && r;

        return r;
    ENDCASE,

    CASE("deserializeBody")
        // setup
        bool r = true;
        std::string str = "7\ntesting";
        Body b = Body::deserialize(str);

        // assert
        r = assertEqual(b.length(), (ssize_t)7) && r;
        r = assertEqual(b.content(), std::string("testing")) && r;
        r = assertEqual(b.serialize(), str) && r;

        return r;
    ENDCASE,

    CASE("constructorFromString")
        // setup
        bool r = true;
        std::string str = "7\ntesting";
        Body b1 = Body::deserialize(str);
        Body b2("testing");

        // assert
        r = assertEqual(b1, b2) && r;

        return r;
    ENDCASE,

    CASE("defaultConstructor")
        // setup
        bool r = true;
        Body b1;

        // assert
        r = assertEqual(b1.content(), std::string("")) && r;
        r = assertEqual(b1.length(), (ssize_t)0) && r;
        r = assertEqual(b1.serialize(), std::string("0\n")) && r;

        return r;
    ENDCASE

ENDTEST

RUNTEST