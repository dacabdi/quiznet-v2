#include "Message.h"
#include "Test.h"

TEST

    CASE("deserializeMessage")
        // setup
        bool r = true;
        std::string str = "a 7\ntesting\n";
        Message m = Message::deserialize(str);

        // assert
        r = assertEqual(m.type(), 'a') && r;
        r = assertEqual(m.typestr(), std::string("a")) && r;
        r = assertEqual(m.serialize(), str) && r;
        r = assertEqual(m.body().length(), (ssize_t)7) && r;
        r = assertEqual(m.body().content(), std::string("testing")) && r;
        
        return r;
    ENDCASE,

    CASE("createFromTypeAndBody")
        // setup
        bool r = true;
        char t = 'a';
        Body b("testing");
        std::string str = "a 7\ntesting\n";
        Message m(t, b);

        // assert
        r = assertEqual(m.type(), 'a') && r;
        r = assertEqual(m.typestr(), std::string("a")) && r;
        r = assertEqual(m.serialize(), str) && r;
        r = assertEqual(m.body().length(), (ssize_t)7) && r;
        r = assertEqual(m.body().content(), std::string("testing")) && r;

        return r;
    ENDCASE,

    CASE("defaultConstructor")
        // setup
        bool r = true;
        Message m;

        // assert
        r = assertEqual(m.type(), '\0') && r;
        r = assertEqual(m.typestr(), std::string("")) && r;
        r = assertEqual(m.serialize(), "") && r;
        r = assertEqual(m.body().length(), (ssize_t)0) && r;
        r = assertEqual(m.body().content(), std::string("")) && r;

        return r;
    ENDCASE,

    CASE("invalidTypeFormatNonSingle")
        // setup
        bool r = true;
        std::string str = "ax 7\ntesting\n";
        // assert
        assertExcept(Message::deserialize(str), r)

        return r;
    ENDCASE,

    CASE("invalidTypeFormatInvalidCharCaps")
        // setup
        bool r = true;
        std::string str = "A 7\ntesting\n";
        // assert
        assertExcept(Message::deserialize(str), r)
        
        return r;
    ENDCASE,

    CASE("invalidTypeFormatInvalidCharNonAlpha")
        // setup
        bool r = true;
        std::string str = "& 7\ntesting\n";
        // assert
        assertExcept(Message::deserialize(str), r)
        
        return r;
    ENDCASE,

    CASE("invalidTypeFormatInvalidMissingSpace")
        // setup
        bool r = true;
        std::string str = "a7\ntesting\n";
        // assert
        assertExcept(Message::deserialize(str), r)
        
        return r;
    ENDCASE

ENDTEST

RUNTEST