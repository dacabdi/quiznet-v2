#include "Test.h"
#include "ContestSession.h"

// test fixture

SolvedQuestion q1(
"presidents, US history\nWhich is the first president of the "
"USA\n.\n(a) Thomas Jefferson\n.\n(b) Abraham Lincoln\n.\n(c) "
"George Washington\n.\n(d) Benjamin Franklin\n.\n.\nc\n");
uint32_t q1_Id = 23; 

SolvedQuestion q2(
"network"
", protocols,layered\nWhich of these protocols guarantee delivery"
" \n.\n(a) TCP\n.\n(b) UDP\n.\n.\na\n");
uint32_t q2_Id = 43;

SolvedQuestion q3(
"application, protocols, "
"DNS\nWhich application protocol provides\nhostname to IP address"
" resolution\n.\n(a) FTP\n.\n(b) HTTP\n.\n(c) DNS\n.\n(d) SMTP\n."
"\n.\nd\n");
uint32_t q3_Id = 54;

SolvedQuestion q4(
"network, protocols,layered\nWhich of these does not b"
"elong in the Internet protocol stack model\n.\n(a) presentation"
"\n.\n(b) transport\n.\n(c) link\n.\n(d) network\n.\n.\na\n");
uint32_t q4_Id = 57;

SolvedQuestion q5(
"architecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
"(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
uint32_t q5_Id = 89;


TEST

    CASE("Provided_Unbound_Socket")
        // data setup
        std::map<uint32_t, SolvedQuestion> questions;
        TcpSocket sock;
        AssertExcept(ContestSession session(sock, questions));
    ENDCASE,

    CASE("Provided_Unbound_Socket")
        // data setup
        std::map<uint32_t, SolvedQuestion> questions;

        questions.emplace(q1_Id, q1);
        questions.emplace(q2_Id, q2);
        questions.emplace(q3_Id, q3);
        questions.emplace(q4_Id, q4);
        questions.emplace(q5_Id, q5);
        
        TcpSocket sock;
        ContestSession session(sock, questions);

    ENDCASE

ENDTEST

RUNTEST