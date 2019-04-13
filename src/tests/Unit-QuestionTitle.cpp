#include "QuestionTitle.h"
#include "Test.h"

TEST

    CASE("QuestionTitleGoldenCase")
        // setup
        std::string s("This is a single line title\n.\n");
        QuestionTitle qt(s);
        std::ostringstream os;
        os << qt;

        //assert
        AssertEqual(qt.getText(), std::string("This is a single line title"));
        AssertEqual(qt.serialize(), s);
        AssertEqual(os.str(), s);
    ENDCASE,

    CASE("QuestionTitleMultilineGoldenCase")
        // setup
        std::string s("A\nmultine\nquestion.\nLook, it has periods!\n.\n");
        QuestionTitle qt(s);
        std::ostringstream os;
        os << qt;

        //assert
        AssertEqual(qt.getText(), std::string("A\nmultine\nquestion.\nLook, it has periods!"));
        AssertEqual(qt.serialize(), s);
        AssertEqual(os.str(), s);
    ENDCASE,

    CASE("QuestionTitleEquality")
        // setup
        std::string s1("A\nmultine\nquestion.\nLook, it has periods!\n.\n");
        std::string s2("A single line question!\n.\n");
        
        QuestionTitle qt1(s1);
        QuestionTitle qt2(s2);
        QuestionTitle qt3(s1);

        //assert
        AssertNotEqual(qt1, qt2);
        AssertNotEqual(qt2, qt1);

        AssertEqual(qt1, qt3);
        AssertEqual(qt3, qt1);
    ENDCASE,

    CASE("QuestionTitleMissingTermination")
        // setup
        std::string s1("A\nmultine\nquestion.\nLook, it has periods!\n.");
        std::string s2("A\nmultine\nquestion.\nLook, it has periods!");
        std::string s3("A\nmultine\nquestion.\nLook, it has periods!.\n");

        AssertExcept(QuestionTitle cc(s1));
        AssertExcept(QuestionTitle cc(s2));
        AssertExcept(QuestionTitle cc(s3));
    ENDCASE,

    CASE("QuestionTitleEmpty")
        // setup
        std::string s1(".\n");
        std::string s2(".\ntesting");

        AssertExcept(QuestionTitle cc(s1));
        AssertExcept(QuestionTitle cc(s2));
    ENDCASE

ENDTEST

RUNTEST
