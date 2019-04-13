#include "Tag.h"
#include "TagCollection.h"
#include "Test.h"

TEST

    CASE("TagGoldenCase")
        // setup
        std::string str = "presidents";
        Tag t(str);

        //assert
        AssertEqual(t.getText(), str);
        AssertEqual(t.serialize(), str);
    ENDCASE,

    CASE("TagCollectionGoldenCase")
        // setup
        std::string input = "presidents,US History, politics";
        TagCollection tc(input);

        //assert
        AssertEqual(tc.at(0).getText(), "presidents");
        AssertEqual(tc.at(1).getText(), "US History");
        AssertEqual(tc.at(2).getText(), " politics");
        AssertEqual(tc.size(), (size_t)3);
        AssertEqual(tc.serialize(), (input + "\n"));

    ENDCASE

ENDTEST

RUNTEST
