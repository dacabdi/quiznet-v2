#include "ProtoError.h"
#include "Test.h"

TEST

    CASE("constructorByDeserialization")
        // setup
        bool r = true;
        
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
        r = assertEqual(e.number(), (const size_t)UNKERR) && r;
        r = assertEqual(e.symbol(), symbol) && r;
        r = assertEqual(e.msg(), message) && r;
        r = assertEqual(e.extra(), extra) && r;
        r = assertEqual(e.present(), presentation) && r;
        r = assertEqual(e.serialize(), expSerialization) && r;

        return r;
    ENDCASE,

    CASE("constructorByParts")
        // setup
        bool r = true;
        
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
        r = assertEqual(e.number(), (const size_t)UNKERR) && r;
        r = assertEqual(e.symbol(), symbol) && r;
        r = assertEqual(e.msg(), message) && r;
        r = assertEqual(e.extra(), extra) && r;
        r = assertEqual(e.present(), presentation) && r;
        r = assertEqual(e.serialize(), expSerialization) && r;

        return r;
    ENDCASE,

    CASE("failedDeserializationMissingNumber")
        // setup
        bool r = true;
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = "\n" + symbol + "\n" + message + "\n.\n" + extra + "\n.\n";

        // assert
        assertExcept(ProtoError e(expSerialization), r);

        return r;
    ENDCASE,

    CASE("failedDeserializationEmpty")
        // setup
        bool r = true;
        std::string expSerialization = "";

        // assert
        assertExcept(ProtoError e(expSerialization), r);

        return r;
    ENDCASE,

    CASE("failedDeserializationMissingMsgTerm")
        // setup
        bool r = true;
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = "\n" + symbol + "\n" + message + "\n." + extra + "\n.\n";

        // assert
        assertExcept(ProtoError e(expSerialization), r);

        return r;
    ENDCASE,

    CASE("failedDeserializationMissingExtraTerm")
        // setup
        bool r = true;
        std::string symbol = "UNKERR";
        std::string message = "Unkown error";
        std::string extra = "Extra information";
        std::string expSerialization = "\n" + symbol + "\n" + message + "\n.\n" + extra + "\n";

        // assert
        assertExcept(ProtoError e(expSerialization), r);

        return r;
    ENDCASE

ENDTEST

RUNTEST