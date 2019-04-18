#include "RoundResults.h"
#include "Test.h"

#include <string>

TEST

    CASE("roundResultsPresent")
        // setup
        RoundResults rr(false, (double)1/(double)4, 3, 1, 1);
        //AssertEqual(rr.present(), "Incorrect. 25% of contestants answered this question correctly.\nYour score is 1/3. The top score is currently 1/3.");
        // (no teardown)
    ENDCASE

ENDTEST

RUNTEST