#include "Choice.h"
#include "ChoiceCollection.h"
#include "Test.h"

TEST

    CASE("ChoiceGoldenCase")
        // setup
        std::string s = "This is a choice";
        Choice choice(s);
        // for stream operator validation
        std::ostringstream oss;
        oss << choice;

        // assert
        AssertEqual(choice.getText(), s);
        AssertEqual(choice.serialize(), s);
        AssertEqual(oss.str(), s);
    ENDCASE,

    CASE("ChoiceEqualsOperator")
        // setup
        
        Choice choice1("Choice 1 and 2 are equal");
        Choice choice2("Choice 1 and 2 are equal");
        Choice choice3("Choice 1 and 3 are not equal");

        // assert (commutative and transitive)
        AssertEqual(choice1, choice2);
        AssertEqual(choice2, choice1);
        AssertNotEqual(choice1, choice3);
        AssertNotEqual(choice2, choice3);
    ENDCASE,

    CASE("ChoiceCollectionGoldenCaseFromString")
        // setup
        std::string input(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            "(c) George Washington\n"
            ".\n"
            "(d) Benjamin Franklin\n"
            ".\n"
            ".\n"
        );
        ChoiceCollection cc(input);
        std::ostringstream oss;
        oss << cc;

        // assert
        AssertEqual(cc.getChoiceById('a'), Choice("Thomas Jefferson"));
        AssertEqual(cc.getChoiceById('b'), Choice("Abraham Lincoln"));
        AssertEqual(cc.getChoiceById('c'), Choice("George Washington"));
        AssertEqual(cc.getChoiceById('d'), Choice("Benjamin Franklin"));

        AssertEqual(cc.size(), (size_t)4);
        AssertEqual(cc.lastChoiceLetter(), 'd');
        AssertEqual(cc.serialize(), input);

        AssertEqual(oss.str(), input);
    ENDCASE,

    CASE("ChoiceCollectionGoldenCaseTwoChoicesString")
        // setup
        std::string input(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );
        ChoiceCollection cc(input);
        std::ostringstream oss;
        oss << cc;

        // assert
        AssertEqual(cc.getChoiceById('a'), Choice("Thomas Jefferson"));
        AssertEqual(cc.getChoiceById('b'), Choice("Abraham Lincoln"));

        AssertEqual(cc.size(), (size_t)2);
        AssertEqual(cc.lastChoiceLetter(), 'b');
        AssertEqual(cc.serialize(), input);

        AssertEqual(oss.str(), input);
    ENDCASE,

    CASE("ChoiceCollectionEqualityOperator")
        // setup
        
        std::string input1(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        std::string input2(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            "(c) Ben Franklim\n"
            ".\n"
            ".\n"
        );

        std::string input3(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Ben Franklim\n"
            ".\n"
            "(c) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        ChoiceCollection cc1(input1);
        ChoiceCollection cc2(input2);
        ChoiceCollection cc3(input3);
        ChoiceCollection cc4(input1);

        // assert

        AssertNotEqual(cc1, cc2);
        AssertNotEqual(cc2, cc1);

        AssertEqual(cc1, cc4);
        AssertEqual(cc4, cc1);

        AssertNotEqual(cc2, cc3);
        AssertNotEqual(cc3, cc2);

        AssertNotEqual(cc3, cc4);
        AssertNotEqual(cc4, cc3);
    ENDCASE,

    CASE("ChoiceCollectionNotEnoughChoices")
        // setup
        
        std::string input(
            "(a) Thomas Jefferson\n"
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionNoChoices")
        // setup
        
        std::string input(
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionChoicesOutOfOrder")
        // setup
        
        std::string input(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(c) Abraham Lincoln\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionChoicesNotAStarting")
        // setup
        
        std::string input(
            "(b) Thomas Jefferson\n"
            ".\n"
            "(c) Abraham Lincoln\n"
            ".\n"
            "(d) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionMissingTerminationPrd")
        // setup
        
        std::string input(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionMissingAllTerminationPrd")
        // setup
        
        std::string input(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionChoiceIdMalformed1")
        // setup
        
        std::string input(
            "(%) Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionChoiceIdMalformed2")
        // setup
        
        std::string input(
            "() Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("ChoiceCollectionChoiceIdMalformed3")
        // setup
        
        std::string input(
            "(a% Thomas Jefferson\n"
            ".\n"
            "(b) Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE,

    CASE("choiceCollectionChoiceIdMalformed4")
        // setup
        
        std::string input(
            "(a) Thomas Jefferson\n"
            ".\n"
            "(b)Abraham Lincoln\n"
            ".\n"
            ".\n"
        );

        AssertExcept(ChoiceCollection cc(input));
    ENDCASE

ENDTEST

RUNTEST
