#include "Test.h"
#include "Contest.h"

SolvedQuestion q1(
"presidents, US history\nWhich is the first president of the "
"USA\n.\n(a) Thomas Jefferson\n.\n(b) Abraham Lincoln\n.\n(c) "
"George Washington\n.\n(d) Benjamin Franklin\n.\n.\nc\n");
uint32_t q1_Id = 1; 

SolvedQuestion q2(
"network"
", protocols,layered\nWhich of these protocols guarantee delivery"
" \n.\n(a) TCP\n.\n(b) UDP\n.\n.\na\n");
uint32_t q2_Id = 2;

SolvedQuestion q3(
"application, protocols, "
"DNS\nWhich application protocol provides\nhostname to IP address"
" resolution\n.\n(a) FTP\n.\n(b) HTTP\n.\n(c) DNS\n.\n(d) SMTP\n."
"\n.\nd\n");
uint32_t q3_Id = 3;

using question_set = std::map<uint32_t, SolvedQuestion>;

TEST

    CASE("contestNormal")

        Contest contest(24);
        contest.addQuestion(q1_Id, q1);
        contest.addQuestion(q2_Id, q2);
        contest.addQuestion(q3_Id, q3);
        contest.setRun();

        // expected stats serialization
        std::string expSerStats = 
        "5\n"     // number of contestants
        "2\n"     // highest score
        "3\n"     // number of questions
        "1 3\n"   // q1id -> q1correct
        "2 2\n"   // ...
        "3 0\n";

        contest.setStats(expSerStats);
        
        std::string expSerialization = 
        "24\n"
        "3\n"
        "true\n"
        "1\n"
        "presidents, US history\n"
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
        "c\n"
        "2\n"
        "network, protocols,layered\n"
        "Which of these protocols guarantee delivery \n"
        ".\n"
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n"
        "a\n"
        "3\n"
        "application, protocols, DNS\n"
        "Which application protocol provides\n"
        "hostname to IP address resolution\n"
        ".\n"
        "(a) FTP\n"
        ".\n"
        "(b) HTTP\n"
        ".\n"
        "(c) DNS\n"
        ".\n"
        "(d) SMTP\n"
        ".\n"
        ".\n"
        "d\n"
        "5\n"
        "2\n"
        "3\n"
        "1 3\n"
        "2 2\n"
        "3 0\n";

        AssertEqual(contest.serialize(), expSerialization);

    ENDCASE,

    CASE("contestNoRun")

        Contest contest(24);
        contest.addQuestion(q1_Id, q1);
        contest.addQuestion(q2_Id, q2);
        contest.addQuestion(q3_Id, q3);

        // expected stats serialization
        /*std::string expSerStats = 
        "5\n"     // number of contestants
        "2\n"     // highest score
        "3\n"     // number of questions
        "1 3\n"   // q1id -> q1correct
        "2 2\n"   // ...
        "3 0\n";*/

        //contest.setStats(expSerStats);
        
        std::string expSerialization = 
        "24\n"
        "3\n"
        "false\n"
        "1\n"
        "presidents, US history\n"
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
        "c\n"
        "2\n"
        "network, protocols,layered\n"
        "Which of these protocols guarantee delivery \n"
        ".\n"
        "(a) TCP\n"
        ".\n"
        "(b) UDP\n"
        ".\n"
        ".\n"
        "a\n"
        "3\n"
        "application, protocols, DNS\n"
        "Which application protocol provides\n"
        "hostname to IP address resolution\n"
        ".\n"
        "(a) FTP\n"
        ".\n"
        "(b) HTTP\n"
        ".\n"
        "(c) DNS\n"
        ".\n"
        "(d) SMTP\n"
        ".\n"
        ".\n"
        "d\n"
        "0\n"
        "0\n"
        "0\n";

        AssertEqual(contest.serialize(), expSerialization);

    ENDCASE

ENDTEST

RUNTEST