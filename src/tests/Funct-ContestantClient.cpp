#include "Test.h"
#include "ContestSession.h"
#include "ContestantClient.h"

#include <thread>

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

    CASE("contestantClientPlays3RoundsAlone")

        std::string input_script(
            "johndoe\n"
            "c\n"
            "b\n"
            "d\n"
        );

        std::istringstream input_script_stream(input_script);
        std::ostringstream output_capture;

        question_set qset;
        qset.emplace(q1_Id, q1);
        qset.emplace(q2_Id, q2);
        qset.emplace(q3_Id, q3);

        ContestSession session(qset);
        Host host = session.getSocket().local();
        ContestantClient client(host, input_script_stream, output_capture, output_capture);

        std::thread client_th([&](){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            client.run();
            //std::cout << output_capture.str();
        });

        std::thread server_th([&](){
            ContestStats stats = session.run(3);
            AssertEqual(stats.average(), .66);
            AssertEqual(stats.contestants(), (uint32_t)1);
            AssertEqual(stats.highest(), (uint32_t)2);
            AssertEqual(stats.max(), (uint32_t)1);
            AssertEqual(stats.questions(), (size_t)3);
        });

        if(client_th.joinable()) client_th.join();
        if(server_th.joinable()) server_th.join();

    ENDCASE,

    CASE("contestantClient2ContestantsPlay3Rounds")

        std::string input_script1(
            "johndoe\n"
            "c\n" // right
            "b\n" // wrong
            "d\n" // right
        );

        std::string input_script2(
            "janedoe\n"
            "a\n" // wrong
            "a\n" // right
            "b\n" // wrong
        );

        std::istringstream input_script_stream1(input_script1);
        std::istringstream input_script_stream2(input_script2);
        std::ostringstream output_capture;

        question_set qset;
        qset.emplace(q1_Id, q1);
        qset.emplace(q2_Id, q2);
        qset.emplace(q3_Id, q3);

        ContestSession session(qset);
        Host host = session.getSocket().local();

        ContestantClient client1(host, input_script_stream1, output_capture, output_capture);
        ContestantClient client2(host, input_script_stream2, output_capture, output_capture);

        std::thread client_th1([&](){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            client1.run();
        });

        std::thread client_th2([&](){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            client2.run();
        });

        std::thread server_th([&](){
            ContestStats stats = session.run(3);
            AssertEqual(stats.average(), .5);
            AssertEqual(stats.contestants(), (uint32_t)2);
            AssertEqual(stats.highest(), (uint32_t)2);
            AssertEqual(stats.max(), (uint32_t)1);
            AssertEqual(stats.questions(), (size_t)3);
        });

        if(client_th1.joinable()) client_th1.join();
        if(client_th2.joinable()) client_th2.join();
        if(server_th.joinable()) server_th.join();

        // std::cout << "Clients output: " << output_capture.str();

    ENDCASE

ENDTEST

RUNTEST