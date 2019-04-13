#include "ProtoError.h"
#include "Test.h"

TEST

    CASE("constructorByDeserialization")
        // setup
        
        
        size_t number = UNKERR;
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = std::to_string(number) + "\n" + symbol + "\n" + message + "\n.\n" + extra + "\n.\n";
        std::string presentation = "Protocol Exception\n"
                                   "Number: 0\n"
                                   "Symbol: UNKERR\n"
                                   "Message: Unkown error\n"
                                   "Extra: Extra information\n";

        ProtoError e(expSerialization);

        // assert
        AssertEqual(e.number(), (const size_t)UNKERR);
        AssertEqual(e.symbol(), symbol);
        AssertEqual(e.msg(), message);
        AssertEqual(e.extra(), extra);
        AssertEqual(e.present(), presentation);
        AssertEqual(e.serialize(), expSerialization);

        
    ENDCASE,

    CASE("constructorByParts")
        // setup
        
        
        size_t number = UNKERR;
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = std::to_string(number) + "\n" + symbol + "\n" + message + "\n.\n" + extra + "\n.\n";
        std::string presentation = "Protocol Exception\n"
                                   "Number: 0\n"
                                   "Symbol: UNKERR\n"
                                   "Message: Unkown error\n"
                                   "Extra: Extra information\n";

        ProtoError e(number, symbol, message, extra);

        // assert
        AssertEqual(e.number(), (const size_t)UNKERR);
        AssertEqual(e.symbol(), symbol);
        AssertEqual(e.msg(), message);
        AssertEqual(e.extra(), extra);
        AssertEqual(e.present(), presentation);
        AssertEqual(e.serialize(), expSerialization);

        
    ENDCASE,

    CASE("failedDeserializationMissingNumber")
        // setup
        
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = "\n" + symbol + "\n" + message + "\n.\n" + extra + "\n.\n";

        // assert
        AssertExcept(ProtoError e(expSerialization));

        
    ENDCASE,

    CASE("failedDeserializationEmpty")
        // setup
        
        std::string expSerialization = "";

        // assert
        AssertExcept(ProtoError e(expSerialization));

        
    ENDCASE,

    CASE("failedDeserializationMissingMsgTerm")
        // setup
        
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = "\n" + symbol + "\n" + message + "\n." + extra + "\n.\n";

        // assert
        AssertExcept(ProtoError e(expSerialization));

        
    ENDCASE,

    CASE("failedDeserializationMissingExtraTerm")
        // setup
        
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = "\n" + symbol + "\n" + message + "\n.\n" + extra + "\n";

        // assert
        AssertExcept(ProtoError e(expSerialization));

        
    ENDCASE

ENDTEST

RUNTEST