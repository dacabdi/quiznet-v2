#include "Message.h"
#include "Test.h"

TEST

    CASE("deserializeMessage")
        // setup
        
        std::string str = "a 7\ntesting\n";
        Message m = Message::deserialize(str);

        // assert
        AssertEqual(m.type(), 'a');
        AssertEqual(m.typestr(), std::string("a"));
        AssertEqual(m.serialize(), str);
        AssertEqual(m.body().length(), (ssize_t)7);
        AssertEqual(m.body().content(), std::string("testing"));
        
        
    ENDCASE,

    CASE("createFromTypeAndBody")
        // setup
        
        char t = 'a';
        Body b("testing");
        std::string str = "a 7\ntesting\n";
        Message m(t, b);

        // assert
        AssertEqual(m.type(), 'a');
        AssertEqual(m.typestr(), std::string("a"));
        AssertEqual(m.serialize(), str);
        AssertEqual(m.body().length(), (ssize_t)7);
        AssertEqual(m.body().content(), std::string("testing"));

        
    ENDCASE,

    CASE("defaultConstructor")
        // setup
        
        Message m;

        // assert
        AssertEqual(m.type(), '\0');
        AssertEqual(m.typestr(), std::string(""));
        AssertEqual(m.serialize(), "");
        AssertEqual(m.body().length(), (ssize_t)0);
        AssertEqual(m.body().content(), std::string(""));

        
    ENDCASE,

    CASE("invalidTypeFormatNonSingle")
        // setup
        
        std::string str = "ax 7\ntesting\n";
        // assert
        AssertExcept(Message::deserialize(str));

        
    ENDCASE,

    CASE("invalidTypeFormatInvalidCharCaps")
        // setup
        
        std::string str = "A 7\ntesting\n";
        // assert
        AssertExcept(Message::deserialize(str));
        
        
    ENDCASE,

    CASE("invalidTypeFormatInvalidCharNonAlpha")
        // setup
        
        std::string str = "& 7\ntesting\n";
        // assert
        AssertExcept(Message::deserialize(str));
        
        
    ENDCASE,

    CASE("invalidTypeFormatInvalidMissingSpace")
        // setup
        
        std::string str = "a7\ntesting\n";
        // assert
        AssertExcept(Message::deserialize(str));
        
        
    ENDCASE

ENDTEST

RUNTEST