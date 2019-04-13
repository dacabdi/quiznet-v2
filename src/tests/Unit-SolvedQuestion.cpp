#include "SolvedQuestion.h"
#include "Test.h"

TEST

    CASE("SolvedQuestionProgrammaticallyGoldenCase")
        //setup
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
            ".\n"
            "c\n");

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

        char solution = 'c';
        Question question(tags, questionTitle, choices);
        SolvedQuestion sq(question, solution);
        
        std::ostringstream os;
        os << sq;

        //assert
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.getQuestion().getQuestionTitle(), questionTitle);
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.serialize(), expectedSerialization);
        AssertEqual(sq.getSolution(), solution);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("SolvedQuestionProgrammaticallyTwoChoices")
        //setup
        std::string expectedSerialization(
            "presidents, US history\n"
            "Which is the first president of the USA\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
            "b\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        char solution = 'b';
        Question question(tags, questionTitle, choices);
        SolvedQuestion sq(question, solution);
        
        std::ostringstream os;
        os << sq;

        //assert
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.getQuestion().getQuestionTitle(), questionTitle);
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.serialize(), expectedSerialization);
        AssertEqual(sq.getSolution(), solution);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("SolvedQuestionProgrammaticallyMultilineTitle")
        //setup
        std::string expectedSerialization(
            "presidents, US history\n"
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
            "a\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which\nis\nthe\nfirst\npresident\nof\nthe\nUSA.\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");

        char solution = 'a';
        Question question(tags, questionTitle, choices);
        SolvedQuestion sq(question, solution);
        
        std::ostringstream os;
        os << sq;

        //assert
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.getQuestion().getQuestionTitle(), questionTitle);
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.serialize(), expectedSerialization);
        AssertEqual(sq.getSolution(), solution);
        AssertEqual(os.str(), expectedSerialization);
    ENDCASE,

    CASE("SolvedQuestionStringGoldenCase")
        //setup
        std::string s("presidents, US history\n"
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
            ".\n"
            "c\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            "(c) George Washington\n"
            ".\n"
            "(d) Benjamin Franklin\n"
            ".\n"
            ".\n");
        
        char solution = 'c';
        SolvedQuestion sq(s);
        
        std::ostringstream os;
        os << sq;

        //assert
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.getQuestion().getQuestionTitle(), questionTitle);
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.serialize(), s);
        AssertEqual(sq.getSolution(), solution);
        AssertEqual(os.str(), s);
    ENDCASE,

    CASE("solvedQuestionStringTwoChoices")
        //setup
        std::string s("presidents, US history\n"
            "Which is the first president of the USA\n"
            ".\n"
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
            "a\n");

        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");
        
        char solution = 'a';
        SolvedQuestion sq(s);
        
        std::ostringstream os;
        os << sq;

        //assert
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.getQuestion().getQuestionTitle(), questionTitle);
        AssertEqual(sq.getQuestion().getTags(), tags);
        AssertEqual(sq.serialize(), s);
        AssertEqual(sq.getSolution(), solution);
        AssertEqual(os.str(), s);
    ENDCASE,

    CASE("SolvedQuestionProgrammaticallySolutionIsNotAChoice")
        //setup
        TagCollection tags("presidents, US history");
        QuestionTitle questionTitle(
            "Which is the first president of the USA\n.\n");
        ChoiceCollection choices(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n");
        Question q(tags, questionTitle, choices);
        char solution = 'c';
        
        AssertExcept(SolvedQuestion sq(q, solution));
    ENDCASE,

    CASE("SolvedQuestionStringSolutionIsNotAChoice")
        //setup
        std::string input(
            "network, protocols, application layer\n"
            "Which protocol provides\nguaranteed delivery\n"
            ".\n"
            "(a) TCP\n"
            ".\n"
            "(b) UDP\n"
            ".\n"
            ".\n"
            "c\n"
        );
        
        AssertExcept(SolvedQuestion sq = SolvedQuestion(input));
    ENDCASE

ENDTEST

RUNTEST
