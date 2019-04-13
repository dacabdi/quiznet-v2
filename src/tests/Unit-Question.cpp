#include "Question.h"
#include "Test.h"

TEST

    CASE("QuestionProgrammaticallyGoldenCase")
        // setup
        std::string expectedSerialization("presidents, US history\n"
            "Which is the first president of the USA\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            "(c) George Washington\n"
            ".\n"
            "(d) Benjamin Franklin\n"
            ".\n"
            ".\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices("(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            "(c) George Washington\n"
            ".\n"
            "(d) Benjamin Franklin\n"
            ".\n"
            ".\n");

        Question question(tags, questionTitle, choices);
        std::ostringstream os;
        os << question;

        //assert
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.getQuestionTitle(), questionTitle);
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.serialize(), expectedSerialization);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("QuestionProgrammaticallyTwoChoices")
        // setup
        std::string expectedSerialization(
            "presidents, US history\n"
            "Which is the first president of the USA\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        Question question(tags, questionTitle, choices);
        std::ostringstream os;
        os << question;

        //assert
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.getQuestionTitle(), questionTitle);
        AssertEqual(question.serialize(), expectedSerialization);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("QuestionProgrammaticallyEquality")
        // setup
        std::string expectedSerialization1(
        "presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        ".\n");

        TagCollection tags1("presidents, US history");
        QuestionTitle questionTitle1(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices1(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        std::string expectedSerialization2(
            "network, protocols, application layer\n"
            "Which protocol provides\nguaranteed delivery\n"
            ".\n"
            "(a) TCP\n"
            ".\n"
            "(b) UDP\n"
            ".\n"
            ".\n");

        TagCollection tags2("network, protocols, application layer");
        QuestionTitle questionTitle2(
            "Which protocol provides\nguaranteed delivery\n"
            ".\n");
        ChoiceCollection choices2(
            "(a) TCP\n"
            ".\n"
            "(b) UDP\n"
            ".\n"
            ".\n");

        Question question1(tags1, questionTitle1, choices1);
        Question question2(tags2, questionTitle2, choices2);
        Question question3(tags1, questionTitle1, choices1);
        Question question4(tags2, questionTitle2, choices1);

        std::ostringstream os1;
        os1 << question1;

        std::ostringstream os2;
        os2 << question2;

        //assert
        AssertEqual(question1.getTags(), tags1);
        AssertEqual(question1.getQuestionTitle(), questionTitle1);
        AssertEqual(question1.serialize(), expectedSerialization1);
        AssertEqual(os1.str(), expectedSerialization1);

        AssertEqual(question2.getTags(), tags2);
        AssertEqual(question2.getQuestionTitle(), questionTitle2);
        AssertEqual(question2.serialize(), expectedSerialization2);
        AssertEqual(os2.str(), expectedSerialization2);

        AssertNotEqual(question1, question2);
        AssertEqual(question1, question3);
        AssertNotEqual(question1, question4);
    ENDCASE,

    CASE("QuestionProgrammaticallyMultilineTitle")
        // setup
        std::string expectedSerialization(
            "presidents, US history\n"
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        Question question(tags, questionTitle, choices);
        std::ostringstream os;
        os << question;

        //assert
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.getQuestionTitle(), questionTitle);
        AssertEqual(question.serialize(), expectedSerialization);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("QuestionStringGoldenCase")
        // setup
        std::string expectedSerialization(
            "presidents, US history\n"
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        Question question(expectedSerialization);
        std::ostringstream os;
        os << question;

        //assert
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.getQuestionTitle(), questionTitle);
        AssertEqual(question.serialize(), expectedSerialization);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("QuestionStringTwoChoices")
        // setup
        std::string expectedSerialization(
            "presidents, US history\n"
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        Question question(expectedSerialization);
        std::ostringstream os;
        os << question;

        //assert
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.getQuestionTitle(), questionTitle);
        AssertEqual(question.serialize(), expectedSerialization);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("QuestionStringEquality")
        // setup
        std::string expectedSerialization1(
            "presidents, US history\n"
            "Which is the first president of the USA\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        TagCollection tags1("presidents, US history");
        QuestionTitle questionTitle1(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices1(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        std::string expectedSerialization2(
            "network, protocols, application layer\n"
            "Which protocol provides\nguaranteed delivery\n"
            ".\n"
            "(a) TCP\n"
            ".\n"
            "(b) UDP\n"
            ".\n"
            ".\n");

        TagCollection tags2("network, protocols, application layer");
        QuestionTitle questionTitle2(
            "Which protocol provides\nguaranteed delivery\n"
            ".\n");
        ChoiceCollection choices2(
            "(a) TCP\n"
            ".\n"
            "(b) UDP\n"
            ".\n"
            ".\n");

        Question question1(expectedSerialization1);
        Question question2(expectedSerialization2);
        Question question3(expectedSerialization1);

        std::ostringstream os1;
        os1 << question1;

        std::ostringstream os2;
        os2 << question2;

        //assert
        AssertEqual(question1.getTags(), tags1);
        AssertEqual(question1.getQuestionTitle(), questionTitle1);
        AssertEqual(question1.serialize(), expectedSerialization1);
        AssertEqual(os1.str(), expectedSerialization1);

        AssertEqual(question2.getTags(), tags2);
        AssertEqual(question2.getQuestionTitle(), questionTitle2);
        AssertEqual(question2.serialize(), expectedSerialization2);
        AssertEqual(os2.str(), expectedSerialization2);

        AssertEqual(question1, question1); //reflex
        AssertNotEqual(question1, question2);
        AssertEqual(question1, question3);
    ENDCASE,

    CASE("QuestionStringMultilineTitle")
        // setup
        std::string expectedSerialization(
            "presidents, US history\n"
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        Question question(expectedSerialization);
        std::ostringstream os;
        os << question;

        //assert
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.getQuestionTitle(), questionTitle);
        AssertEqual(question.serialize(), expectedSerialization);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("QuestionStringEmptyTags")
        // setup
        std::string expectedSerialization(
            "\n"
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        TagCollection tags("");
        QuestionTitle questionTitle(
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        Question question(expectedSerialization);
        std::ostringstream os;
        os << question;

        //assert
        AssertEqual(question.getTags(), tags);
        AssertEqual(question.getQuestionTitle(), questionTitle);
        AssertEqual(question.serialize(), expectedSerialization);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("QuestionStringLessThanTwoChoices")
        // setup
        std::string input(
            "network, protocols, application layer\n"
            "Which protocol provides\nguaranteed delivery\n"
            ".\n"
            "(a) TCP\n"
            ".\n"
            ".\n");

        AssertExcept(Question question(input));
    ENDCASE,

    CASE("QuestionStringNoChoices")
        // setup
        std::string input(
            "network, protocols, application layer\n"
            "Which protocol provides\nguaranteed delivery\n"
            ".\n"
            ".\n"
            ".\n");

        AssertExcept(Question question(input));
    ENDCASE,

    CASE("QuestionStringUnterminatedQuestionTitle")
        // setup
        std::string input(
            "network, protocols, application layer\n"
            "Which protocol provides\nguaranteed delivery"
            "(a) TCP\n"
            ".\n"
            "(b) UDP\n"
            ".\n"
            ".\n");

        AssertExcept(Question question(input));
    ENDCASE,

    CASE("QuestionStringUnterminatedChoices")
        // setup
        std::string input(
            "network, protocols, application layer\n"
            "Which protocol provides\nguaranteed delivery"
            "(a) TCP\n"
            ".\n"
            "(b) UDP\n"
            ".\n"
            ".\n");

        AssertExcept(Question question(input));
    ENDCASE

ENDTEST

RUNTEST
