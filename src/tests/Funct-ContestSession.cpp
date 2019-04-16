#include "Test.h"
#include "Utils.h"
#include "Question.h"
#include "RoundResults.h"
#include "ContestSession.h"
#include "UniformRandom.h"

#include <algorithm>

// test fixture

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

SolvedQuestion q4(
"network, protocols,layered\nWhich of these does not b"
"elong in the Internet protocol stack model\n.\n(a) presentation"
"\n.\n(b) transport\n.\n(c) link\n.\n(d) network\n.\n.\na\n");
uint32_t q4_Id = 4;

SolvedQuestion q5(
"architecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
"(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
uint32_t q5_Id = 5;

SolvedQuestion q6(
"architecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
"(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
uint32_t q6_Id = 6;

SolvedQuestion q7(
"architecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
"(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
uint32_t q7_Id = 7;

SolvedQuestion q8(
"architecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
"(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
uint32_t q8_Id = 8;


std::vector<std::string> ____name_set = {
    // typical centaurs
    "Agrius",
    "Amycus",
    "Asbolus",
    "Bienor",
    "Centaurus",
    "Chiron",
    "Chthonius",
    "Cyllarus",
    "Dictys",
    "Eurytus",
    "Eurynomos",
    "Elatus",
    "Eurytion",
    "Hylaeus",
    "Hylonome",
    "Nessus",
    "Perimedes",
    "Pholos",
    "Pholus",
    "Rhoetus",
    "Thaumas",
    "Rhaecus",
    // lamian centaurs
    "Aescaus",
    "Amphithemis",
    "Ceteus",
    "Eurybios",
    "Faunus",
    "Gleneus",
    "Orthaon",
    "Petraeus"
};

TEST

    CASE("sessionlisteningAndInitSocket")
        // data setup
        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);

        ContestSession cs(qs);

        // assert
        AssertNotEqual(cs.getSocket().fd(), -1);
        AssertEqual(cs.getSocket().bound(), true);
    ENDCASE,

    CASE("gatherContestants_dropOnNegotiatingNckRetry")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  A contestant drops the connection during session negotiation nickname retry,
        //      THEN  The session ignores the client

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);   

        size_t timeout = 1;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::string nickname = "johndoe";

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session host is ready
            
            cs.StartSession(timeout);
            std::vector<std::string> namesRead = cs.getNames();

            AssertEqual(namesRead.size(), (size_t)1);
            AssertEqual(namesRead.at(0), nickname);
        });

        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            // first contestant
            TcpSocket conn1;
            conn1.Connect(sessionHost);
            conn1.write(Message('n', Body("contestant\n" + nickname + "\n")).serialize());
            Message res1(conn1.read());
            
            AssertEqual(res1.type(), 'o'); // the response should be OK

            // second contestant
            TcpSocket conn2;
            conn2.Connect(sessionHost);
            conn2.write(Message('n', Body("contestant\n" + nickname + "\n")).serialize());

            // response must be an error of type NCKDUP
            Message res2(conn2.read());
            AssertEqual(res2.type(), 'e');
            AssertEqual((int)ProtoError(res2.body().content()).number(), NCKDUP);

            // close the socket and stop trying
            conn2.Close();
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_dropOnBeforeTxNickname")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  A contestant connects but drops before sending nick
        //      THEN  The session ignores the client

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);   

        size_t timeout = 1;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session host is ready
            
            cs.StartSession(timeout);
            std::vector<std::string> namesRead = cs.getNames();

            AssertEqual(namesRead.size(), (size_t)0);
        });

        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            // first contestant
            TcpSocket conn;
            conn.Connect(sessionHost);
            conn.Close();
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_oneDuplicateContestant")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Join duplicate contestants before timeout,
        //      THEN  An error and a retry are given

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);   

        size_t timeout = 1;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::string nickname1 = "johndoe";
        std::string nickname2 = "janedoe";

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session host is ready
            
            cs.StartSession(timeout);
            std::vector<std::string> namesRead = cs.getNames();

            AssertEqual(namesRead.size(), (size_t)2);
            AssertEqual(namesRead.at(0), nickname2);
            AssertEqual(namesRead.at(1), nickname1);
        });

        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            // first contestant
            TcpSocket conn1;
            conn1.Connect(sessionHost);
            conn1.write(Message('n', Body("contestant\n" + nickname1 + "\n")).serialize());
            Message res1(conn1.read());
            
            AssertEqual(res1.type(), 'o'); // the response should be OK

            // second contestant
            TcpSocket conn2;
            conn2.Connect(sessionHost);
            conn2.write(Message('n', Body("contestant\n" + nickname1 + "\n")).serialize());

            // response must be an error of type NCKDUP
            Message res2(conn2.read());
            AssertEqual(res2.type(), 'e');
            AssertEqual((int)ProtoError(res2.body().content()).number(), NCKDUP);

            // retry with different nickname
            conn2.write(Message('n', Body("contestant\n" + nickname2 + "\n")).serialize());
            res2 = Message(conn2.read());
            AssertEqual(res2.type(), 'o');
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_threeDuplicateContestants")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Three duplicates are negotiated with a retry
        //      THEN  All cases are registered
        
        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);

        size_t timeout = 2;

        std::vector<std::string>    names {"a", "b", "c"};
        std::vector<std::string> altNames {"d", "e", "f"};
    
        std::vector<std::thread> cths;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session is ready

            cs.StartSession(timeout);
            std::vector<std::string> namesRead = cs.getNames();

            // couple names
            names.insert(names.end(), altNames.begin(), altNames.end());

            // sort everything
            std::sort(names.begin(), names.end());
            std::sort(namesRead.begin(), namesRead.end());

            AssertEqual(namesRead, names); // all names must match
        });

        // original names threads
        for (auto &name : names) cths.push_back(std::thread([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // session host is ready
            
            TcpSocket conn;
            conn.Connect(sessionHost);
            conn.write(Message('n', Body("contestant\n" + name + "\n")).serialize());
            Message res(conn.read());

            AssertEqual(res.type(), 'o'); // all responses must be OK
        }));

        // join first chance names threads
        for (auto &cthread : cths) if(cthread.joinable()) cthread.join();

        // now launch all duplicates
        for (size_t i = 0; i < names.size(); i++) cths.push_back(std::thread([&](size_t i){ 
            // no need to check for host ready
            TcpSocket conn;
            conn.Connect(sessionHost);
            
            // send duplicate name
            conn.write(Message('n', Body("contestant\n" + names[i] + "\n")).serialize());
        
            // response must be an error of type NCKDUP
            Message res(conn.read());
            AssertEqual(res.type(), 'e');
            AssertEqual((int)ProtoError(res.body().content()).number(), NCKDUP);

            // send substitute name
            conn.write(Message('n', Body("contestant\n" + altNames[i] + "\n")).serialize());
            AssertEqual(Message(conn.read()).type(), 'o'); // must be OK
        }, i));

        // join all threads
        for (auto &cthread : cths) if(cthread.joinable()) cthread.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_noContestant")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Wait for timeout without joining
        //      THEN  No contestants in the session

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);

        size_t timeout = 1;

        std::thread cs([&](){
            ContestSession cs(qs);
            cs.StartSession(timeout);
            AssertEqual(cs.getNames().size(), (size_t)0);
        });

        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_oneContestant")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Join one contestant before timeout
        //      THEN  The session host responds with OK 
        //            And the contestant was registered

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);   

        size_t timeout = 1;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::string nickname = "johndoe";

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session host is ready
            
            cs.StartSession(timeout);
            std::vector<std::string> namesRead = cs.getNames();

            AssertEqual(namesRead.size(), (size_t)1);
            AssertEqual(namesRead.at(0), nickname);
        });

        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            TcpSocket conn;
            conn.Connect(sessionHost);
            conn.write(Message('n', Body("contestant\n" + nickname + "\n")).serialize());
            Message res(conn.read());
            
            AssertEqual(res.type(), 'o'); // the response should be OK
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_manyContestants")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Attempt to join ~50 contestants with distinct names
        //      THEN  All contestants are in
        
        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        
        // generate names A-Z and a-z
        std::vector<std::string> names;
        for(char c = 'A'; c <= 'Z'; c++) names.push_back(std::string(1, c));
        for(char c = 'a'; c <= 'z'; c++) names.push_back(std::string(1, c));

        size_t timeout = 5; // discretionary time to join all contestants
        
        std::vector<std::thread> cths; // contestant threads

        // sync host session ready to take contestants
        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all();

            cs.StartSession(timeout);
            std::vector<std::string> namesRead = cs.getNames();
            
            std::sort(names.begin(), names.end());
            std::sort(namesRead.begin(), namesRead.end());
            
            AssertEqual(namesRead, names); // all names, in lexicographic order, must match
        });

        for (const std::string& name : names)
            cths.push_back(std::thread([&](){
                { // wait for host to be ready
                    std::unique_lock<std::mutex> l(host_mutex);
                    while (!hostReady) cv.wait(l);
                } // host is ready
                
                TcpSocket conn;
                conn.Connect(sessionHost);
                conn.write(Message('n', Body("contestant\n" + name + "\n")).serialize());
                Message response(conn.read());
                
                AssertEqual(response.type(), 'o'); // response must be OK
            }));

        // join all threads
        for (auto &cthread : cths)
            if(cthread.joinable()) cthread.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_attemptJoinAfterClosed")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Attempt to join with after time out
        //      THEN  Socket gets connection denied exception

        std::map<uint32_t, SolvedQuestion> qs; // questions
        qs.emplace(q1_Id, q1);

        size_t timeout = 1; // gather contestants timeout

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
                
            }
            cv.notify_all();
            cs.StartSession(timeout);
            AssertEqual(cs.getNames().size(), (size_t)0); // zero contestants read
        });

        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            // wait a second beyond timeout
            std::this_thread::sleep_for(std::chrono::seconds(timeout+1));

            TcpSocket conn;
            AssertExcept(conn.Connect(sessionHost)); // trying to connect now will get connection refused
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_badRequestsNoClient")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Attempt to join with an invalid handshake format (missing client type)
        //      THEN  Session returns a FAILGT (Failed to negotiate session) error response
        //            The contestant is not registered

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        qs.emplace(q2_Id, q2);
        
        size_t timeout = 2; // enough time for test

        std::mutex host_mutex; // mutex for host setup
        std::condition_variable cv; // condition variable for host ready
        bool hostReady = false; // host ready flag
        Host sessionHost; // for clients to connect

        // start contest session thread
        std::thread cs([&]()
        {
            ContestSession cs(qs);

            { // announce to the other threads that the host is ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
                
            } cv.notify_all();

            // start accepting contestants for 'timeout' seconds
            cs.StartSession(timeout);
            
            // no contestant should have gathered
            AssertEqual(cs.getNames().size(), (size_t)0);
        });
        
        // contestant attempt thread
        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready
        
            TcpSocket conn; // create connection socket
            conn.Connect(sessionHost); // attempt to connec to to contest
            conn.write(Message('n', Body("\n" + std::string("invalid") + "\n")).serialize()); // send handshake with invalid format
            
            Message res(conn.read()); // read response
            AssertEqual(res.type(), 'e'); // response is type error
            AssertEqual((int)(ProtoError::deserialize(res.body().content()).number()), FAILGT); // the protocol error is type NCKDUP
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("gatherContestants_badRequestsEmptyNickname")
        // SUMMARY
        //      GIVEN A contest session on any given port
        //      WHEN  Attempt to join with an invalid handshake format (empty nickname)
        //      THEN  Session returns a FAILGT (Failed to negotiate session) error response
        //            The contestant is not registered

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        qs.emplace(q2_Id, q2);
        
        size_t timeout = 2; // enough time for test

        std::mutex host_mutex; // mutex for host setup
        std::condition_variable cv; // condition variable for host ready
        bool hostReady = false; // host ready flag
        Host sessionHost; // for clients to connect

        // start contest session thread
        std::thread cs([&]()
        {
            ContestSession cs(qs);

            { // announce to the other threads that the host is ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
                
            } cv.notify_all();

            // start accepting contestants for 'timeout' seconds
            cs.StartSession(timeout);
            
            // no contestant should have gathered
            AssertEqual(cs.getNames().size(), (size_t)0);
        });
        
        // contestant attempt thread
        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready
        
            TcpSocket conn; // create connection socket
            conn.Connect(sessionHost); // attempt to connec to to contest
            conn.write(Message('n', Body(std::string("contestant") + "\n\n")).serialize()); // send handshake with invalid format
            
            Message res(conn.read()); // read response
            AssertEqual(res.type(), 'e'); // response is type error
            AssertEqual((int)(ProtoError::deserialize(res.body().content()).number()), FAILGT); // the protocol error is type NCKDUP
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("playRound_oneContestantOneRound")
        // SUMMARY
        //      GIVEN A contest session on any given port with one question
        //      WHEN  Join one contestant before timeout
        //      THEN  The contestant can play for a round

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);   
        
        size_t timeout = 2;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::string nickname = "johndoe";

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session host is ready
            
            AssertEqual(cs.getState(), READY);
            cs.StartSession(timeout);
            AssertEqual(cs.getState(), RUNNING);

            std::vector<std::string> namesRead = cs.getNames();
            AssertEqual(namesRead.size(), (size_t)1);
            AssertEqual(namesRead.at(0), nickname);

            cs.PlayRound(q1_Id);
            cs.TerminateSession();

            AssertEqual(cs.getState(), TERMINATED);

            ContestStats stats = cs.getStats();
            AssertEqual(stats.average(), (double)1.0);
            AssertEqual(stats.contestants(), (uint32_t)1);
            AssertEqual((uint32_t)stats.questions(), (uint32_t)1);
            AssertEqual(stats.max(), (uint32_t)1);
        });

        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            TcpSocket conn;
            conn.Connect(sessionHost);
            conn.write(Message('n', Body("contestant\n" + nickname + "\n")).serialize());
            Message res(conn.read());
            
            AssertEqual(res.type(), 'o'); // the response should be OK

            // logged in ... now wait for question

            // round 1 (indented for clarity)
                Message r1_q_msg(conn.read());
                AssertEqual(r1_q_msg.type(), 'u');

                Question r1_q(r1_q_msg.body().content());
                AssertEqual(r1_q, q1.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, q1.getSolution())).serialize());
                
                // read result
                Message r1_r_msg(conn.read());
                AssertEqual(r1_r_msg.type(), 'u');
                
                // round results
                RoundResults r1_r_rr;
                AssertNotExcept(r1_r_rr = RoundResults::deserialize(r1_r_msg.body().content()));
                AssertEqual(r1_r_rr.correct(), true);
                AssertEqual(r1_r_rr.ratio(), 1.);
                AssertEqual(r1_r_rr.questions(), (uint32_t)1);
                AssertEqual(r1_r_rr.score(), (uint32_t)1);
                AssertEqual(r1_r_rr.max(), (uint32_t)1);

            // wait for termination message or next question (in this case, termination)
                Message t_msg(conn.read());
                AssertEqual(t_msg.type(), 't');

                conn.write(Message('o', "").serialize()); // ack
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("playRound_oneContestantThreeRounds")
        // SUMMARY
        //      GIVEN A contest session on any given port with three questions
        //      WHEN  Join one contestant before timeout
        //      THEN  The contestant can play for three rounds round

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        qs.emplace(q2_Id, q2);
        qs.emplace(q3_Id, q3);
        
        size_t timeout = 2;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::string nickname = "johndoe";

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session host is ready
            
            AssertEqual(cs.getState(), READY);
            cs.StartSession(timeout);
            AssertEqual(cs.getState(), RUNNING);

            std::vector<std::string> namesRead = cs.getNames();
            AssertEqual(namesRead.size(), (size_t)1);
            AssertEqual(namesRead.at(0), nickname);

            cs.PlayRound(q1_Id);
            cs.PlayRound(q2_Id);
            cs.PlayRound(q3_Id);

            cs.TerminateSession();

            AssertEqual(cs.getState(), TERMINATED);

            ContestStats stats = cs.getStats();
            AssertEqual(stats.average(), (double)2/(double)3);
            AssertEqual(stats.contestants(), (uint32_t)1);
            AssertEqual((uint32_t)stats.questions(), (uint32_t)3);
            AssertEqual(stats.max(), (uint32_t)1);
            AssertEqual(stats.highest(), (uint32_t)2);
        });

        std::thread ct([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            TcpSocket conn;
            conn.Connect(sessionHost);
            conn.write(Message('n', Body("contestant\n" + nickname + "\n")).serialize());
            Message res(conn.read());
            
            AssertEqual(res.type(), 'o'); // the response should be OK

            // logged in ... now wait for question

            // round 1 (indented for clarity)
                Message r1_q_msg(conn.read());
                AssertEqual(r1_q_msg.type(), 'u');

                Question r1_q(r1_q_msg.body().content());
                AssertEqual(r1_q, q1.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, q1.getSolution())).serialize());
                
                // read result
                Message r1_r_msg(conn.read());
                AssertEqual(r1_r_msg.type(), 'u');
                
                // round results
                RoundResults r1_r_rr;
                AssertNotExcept(r1_r_rr = RoundResults::deserialize(r1_r_msg.body().content()));
                AssertEqual(r1_r_rr.correct(), true);
                AssertEqual(r1_r_rr.ratio(), 1.);
                AssertEqual(r1_r_rr.questions(), (uint32_t)3);
                AssertEqual(r1_r_rr.score(), (uint32_t)1);
                AssertEqual(r1_r_rr.max(), (uint32_t)1);

            // round 2 (indented for clarity)
                Message r2_q_msg(conn.read());
                AssertEqual(r2_q_msg.type(), 'u');

                Question r2_q(r2_q_msg.body().content());
                AssertEqual(r2_q, q2.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, q2.getSolution())).serialize());
                
                // read result
                Message r2_r_msg(conn.read());
                AssertEqual(r2_r_msg.type(), 'u');
                
                // round results
                RoundResults r2_r_rr;
                AssertNotExcept(r2_r_rr = RoundResults::deserialize(r2_r_msg.body().content()));
                AssertEqual(r2_r_rr.correct(), true);
                AssertEqual(r2_r_rr.ratio(), 1.);
                AssertEqual(r2_r_rr.questions(), (uint32_t)3);
                AssertEqual(r2_r_rr.score(), (uint32_t)2);
                AssertEqual(r2_r_rr.max(), (uint32_t)2);

            // round 3 (indented for clarity)
                Message r3_q_msg(conn.read());
                AssertEqual(r3_q_msg.type(), 'u');

                Question r3_q(r3_q_msg.body().content());
                AssertEqual(r3_q, q3.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, (char)(q3.getSolution()+1))).serialize());
                
                // read result
                Message r3_r_msg(conn.read());
                AssertEqual(r3_r_msg.type(), 'u');
                
                // round results
                RoundResults r3_r_rr;
                AssertNotExcept(r3_r_rr = RoundResults::deserialize(r3_r_msg.body().content()));
                AssertEqual(r3_r_rr.correct(), false);
                AssertEqual(r3_r_rr.ratio(), .0); // nobody got it right in this round
                AssertEqual(r3_r_rr.questions(), (uint32_t)3);
                AssertEqual(r3_r_rr.score(), (uint32_t)2);
                AssertEqual(r3_r_rr.max(), (uint32_t)2);

            // wait for termination message or next question (in this case, termination)
                Message t_msg(conn.read());
                AssertEqual(t_msg.type(), 't');

                conn.write(Message('o', "").serialize()); // ack
        });

        // join all threads
        if (ct.joinable()) ct.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("playRound_twoContestantsThreeRounds")
        // SUMMARY
        //      GIVEN A contest session on any given port with three questions
        //      WHEN  Join two contestants before timeout
        //      THEN  The two contestants can play for three rounds round

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        qs.emplace(q2_Id, q2);
        qs.emplace(q3_Id, q3);
        
        size_t timeout = 3;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::string nickname1 = "johndoe";
        std::string nickname2 = "janedoe";

        std::thread cs([&](){
            ContestSession cs(qs);
            { // announce host ready
                std::unique_lock<std::mutex> l(host_mutex);
                sessionHost = cs.getSocket().local();
                hostReady = true;
            } cv.notify_all(); // session host is ready
            
            AssertEqual(cs.getState(), READY);
            cs.StartSession(timeout);
            AssertEqual(cs.getState(), RUNNING);

            std::vector<std::string> namesRead = cs.getNames();
            AssertEqual(namesRead.size(), (size_t)2);

            std::sort(namesRead.begin(), namesRead.end());
            AssertEqual(namesRead.at(0), nickname2); // jane
            AssertEqual(namesRead.at(1), nickname1); // john

            cs.PlayRound(q1_Id);
            cs.PlayRound(q2_Id);
            cs.PlayRound(q3_Id);

            cs.TerminateSession();

            AssertEqual(cs.getState(), TERMINATED);

            ContestStats stats = cs.getStats();
            AssertEqual(stats.average(), (double)2/(double)3); // TODO CALCULATE
            AssertEqual(stats.contestants(), (uint32_t)2);
            AssertEqual((uint32_t)stats.questions(), (uint32_t)3);
            AssertEqual(stats.max(), (uint32_t)2);
            AssertEqual(stats.highest(), (uint32_t)2);
        });

        std::thread ct1([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            TcpSocket conn;
            conn.Connect(sessionHost);
            conn.write(Message('n', Body("contestant\n" + nickname1 + "\n")).serialize());
            Message res(conn.read());
            
            AssertEqual(res.type(), 'o'); // the response should be OK

            // logged in ... now wait for question

            // round 1 (indented for clarity)
                Message r1_q_msg(conn.read());
                AssertEqual(r1_q_msg.type(), 'u');

                Question r1_q(r1_q_msg.body().content());
                AssertEqual(r1_q, q1.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, q1.getSolution())).serialize());
                
                // read result
                Message r1_r_msg(conn.read());
                AssertEqual(r1_r_msg.type(), 'u');
                
                // round results
                RoundResults r1_r_rr;
                AssertNotExcept(r1_r_rr = RoundResults::deserialize(r1_r_msg.body().content()));
                AssertEqual(r1_r_rr.correct(), true);
                AssertEqual(r1_r_rr.ratio(), 1.0); // both got it right
                AssertEqual(r1_r_rr.questions(), (uint32_t)3);
                AssertEqual(r1_r_rr.score(), (uint32_t)1);
                AssertEqual(r1_r_rr.max(), (uint32_t)1);

            // round 2 (indented for clarity)
                Message r2_q_msg(conn.read());
                AssertEqual(r2_q_msg.type(), 'u');

                Question r2_q(r2_q_msg.body().content());
                AssertEqual(r2_q, q2.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, q2.getSolution())).serialize());
                
                // read result
                Message r2_r_msg(conn.read());
                AssertEqual(r2_r_msg.type(), 'u');
                
                // round results
                RoundResults r2_r_rr;
                AssertNotExcept(r2_r_rr = RoundResults::deserialize(r2_r_msg.body().content()));
                AssertEqual(r2_r_rr.correct(), true);
                AssertEqual(r2_r_rr.ratio(), .5); // i got right, the other got it wrong
                AssertEqual(r2_r_rr.questions(), (uint32_t)3);
                AssertEqual(r2_r_rr.score(), (uint32_t)2);
                AssertEqual(r2_r_rr.max(), (uint32_t)2);

            // round 3 (indented for clarity)
                Message r3_q_msg(conn.read());
                AssertEqual(r3_q_msg.type(), 'u');

                Question r3_q(r3_q_msg.body().content());
                AssertEqual(r3_q, q3.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, (char)(q3.getSolution()+1))).serialize());
                
                // read result
                Message r3_r_msg(conn.read());
                AssertEqual(r3_r_msg.type(), 'u');
                
                // round results
                RoundResults r3_r_rr;
                AssertNotExcept(r3_r_rr = RoundResults::deserialize(r3_r_msg.body().content()));
                AssertEqual(r3_r_rr.correct(), false);
                AssertEqual(r3_r_rr.ratio(), .5); // i wrong right, the other got it right
                AssertEqual(r3_r_rr.questions(), (uint32_t)3);
                AssertEqual(r3_r_rr.score(), (uint32_t)2);
                AssertEqual(r3_r_rr.max(), (uint32_t)2);

            // wait for termination message or next question (in this case, termination)
                Message t_msg(conn.read());
                AssertEqual(t_msg.type(), 't');

                conn.write(Message('o', "").serialize()); // ack
        });

        std::thread ct2([&](){
            { // wait for host to be ready
                std::unique_lock<std::mutex> l(host_mutex);
                while (!hostReady) cv.wait(l);
            } // host is ready

            TcpSocket conn;
            conn.Connect(sessionHost);
            conn.write(Message('n', Body("contestant\n" + nickname2 + "\n")).serialize());
            Message res(conn.read());
            
            AssertEqual(res.type(), 'o'); // the response should be OK

            // logged in ... now wait for question

            // round 1 (indented for clarity)
                Message r1_q_msg(conn.read());
                AssertEqual(r1_q_msg.type(), 'u');

                Question r1_q(r1_q_msg.body().content());
                AssertEqual(r1_q, q1.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, q1.getSolution())).serialize());
                
                // read result
                Message r1_r_msg(conn.read());
                AssertEqual(r1_r_msg.type(), 'u');
                
                // round results
                RoundResults r1_r_rr;
                AssertNotExcept(r1_r_rr = RoundResults::deserialize(r1_r_msg.body().content()));
                AssertEqual(r1_r_rr.correct(), true);
                AssertEqual(r1_r_rr.ratio(), 1.);
                AssertEqual(r1_r_rr.questions(), (uint32_t)3);
                AssertEqual(r1_r_rr.score(), (uint32_t)1);
                AssertEqual(r1_r_rr.max(), (uint32_t)1);

            // round 2 (indented for clarity)
                Message r2_q_msg(conn.read());
                AssertEqual(r2_q_msg.type(), 'u');

                Question r2_q(r2_q_msg.body().content());
                AssertEqual(r2_q, q2.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, (char)(q3.getSolution()+1))).serialize());
                
                // read result
                Message r2_r_msg(conn.read());
                AssertEqual(r2_r_msg.type(), 'u');
                
                // round results
                RoundResults r2_r_rr;
                AssertNotExcept(r2_r_rr = RoundResults::deserialize(r2_r_msg.body().content()));
                AssertEqual(r2_r_rr.correct(), false);
                AssertEqual(r2_r_rr.ratio(), .5);
                AssertEqual(r2_r_rr.questions(), (uint32_t)3);
                AssertEqual(r2_r_rr.score(), (uint32_t)1);
                AssertEqual(r2_r_rr.max(), (uint32_t)2);

            // round 3 (indented for clarity)
                Message r3_q_msg(conn.read());
                AssertEqual(r3_q_msg.type(), 'u');

                Question r3_q(r3_q_msg.body().content());
                AssertEqual(r3_q, q3.getQuestion());

                // send response
                conn.write(Message('u', std::string(1, q3.getSolution())).serialize());
                
                // read result
                Message r3_r_msg(conn.read());
                AssertEqual(r3_r_msg.type(), 'u');
                
                // round results
                RoundResults r3_r_rr;
                AssertNotExcept(r3_r_rr = RoundResults::deserialize(r3_r_msg.body().content()));
                AssertEqual(r3_r_rr.correct(), true);
                AssertEqual(r3_r_rr.ratio(), .5);
                AssertEqual(r3_r_rr.questions(), (uint32_t)3);
                AssertEqual(r3_r_rr.score(), (uint32_t)2);
                AssertEqual(r3_r_rr.max(), (uint32_t)2);

            // wait for termination message or next question (in this case, termination)
                Message t_msg(conn.read());
                AssertEqual(t_msg.type(), 't');

                conn.write(Message('o', "").serialize()); // ack
        });

        // join all threads
        if (ct1.joinable()) ct1.join();
        if (ct2.joinable()) ct2.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("playRound_twoContestantsThreeRounds20Times") // abusing timing issues
    for(size_t i = 0; i < 20; i++) {
        // SUMMARY
        //      GIVEN A contest session on any given port with three questions
        //      WHEN  Join two contestants before timeout
        //      THEN  The two contestants can play for three rounds round

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        qs.emplace(q2_Id, q2);
        qs.emplace(q3_Id, q3);
        
        size_t timeout = 3;

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::string nickname1 = "johndoe";
        std::string nickname2 = "janedoe";

        std::thread cs([&](){
            try {
                ContestSession cs(qs);
                { // announce host ready
                    std::unique_lock<std::mutex> l(host_mutex);
                    sessionHost = cs.getSocket().local();
                    hostReady = true;
                } cv.notify_all(); // session host is ready
                
                AssertEqual(cs.getState(), READY);
                cs.StartSession(timeout);
                AssertEqual(cs.getState(), RUNNING);

                std::vector<std::string> namesRead = cs.getNames();
                AssertEqual(namesRead.size(), (size_t)2);

                std::sort(namesRead.begin(), namesRead.end());
                AssertEqual(namesRead.at(0), nickname2); // jane
                AssertEqual(namesRead.at(1), nickname1); // john

                cs.PlayRound(q1_Id);
                cs.PlayRound(q2_Id);
                cs.PlayRound(q3_Id);

                cs.TerminateSession();

                AssertEqual(cs.getState(), TERMINATED);

                ContestStats stats = cs.getStats();
                AssertEqual(stats.average(), (double)2/(double)3); // TODO CALCULATE
                AssertEqual(stats.contestants(), (uint32_t)2);
                AssertEqual((uint32_t)stats.questions(), (uint32_t)3);
                AssertEqual(stats.max(), (uint32_t)2);
                AssertEqual(stats.highest(), (uint32_t)2);
            } catch(const std::exception& e) {
                std::cout << "Exception in contest session thread : " << e.what() << std::endl;
            }
        });

        std::thread ct1([&](){
            try {
                { // wait for host to be ready
                    std::unique_lock<std::mutex> l(host_mutex);
                    while (!hostReady) cv.wait(l);
                } // host is ready

                TcpSocket conn;
                conn.Connect(sessionHost);
                conn.write(Message('n', Body("contestant\n" + nickname1 + "\n")).serialize());
                Message res(conn.read());
                
                AssertEqual(res.type(), 'o'); // the response should be OK

                // logged in ... now wait for question

                // round 1 (indented for clarity)
                    Message r1_q_msg(conn.read());
                    AssertEqual(r1_q_msg.type(), 'u');

                    Question r1_q(r1_q_msg.body().content());
                    AssertEqual(r1_q, q1.getQuestion());

                    // send response
                    conn.write(Message('u', std::string(1, q1.getSolution())).serialize());
                    
                    // read result
                    Message r1_r_msg(conn.read());
                    AssertEqual(r1_r_msg.type(), 'u');
                    
                    // round results
                    RoundResults r1_r_rr;
                    AssertNotExcept(r1_r_rr = RoundResults::deserialize(r1_r_msg.body().content()));
                    AssertEqual(r1_r_rr.correct(), true);
                    AssertEqual(r1_r_rr.ratio(), 1.0); // both got it right
                    AssertEqual(r1_r_rr.questions(), (uint32_t)3);
                    AssertEqual(r1_r_rr.score(), (uint32_t)1);
                    AssertEqual(r1_r_rr.max(), (uint32_t)1);

                // round 2 (indented for clarity)
                    Message r2_q_msg(conn.read());
                    AssertEqual(r2_q_msg.type(), 'u');

                    Question r2_q(r2_q_msg.body().content());
                    AssertEqual(r2_q, q2.getQuestion());

                    // send response
                    conn.write(Message('u', std::string(1, q2.getSolution())).serialize());
                    
                    // read result
                    Message r2_r_msg(conn.read());
                    AssertEqual(r2_r_msg.type(), 'u');
                    
                    // round results
                    RoundResults r2_r_rr;
                    AssertNotExcept(r2_r_rr = RoundResults::deserialize(r2_r_msg.body().content()));
                    AssertEqual(r2_r_rr.correct(), true);
                    AssertEqual(r2_r_rr.ratio(), .5); // i got right, the other got it wrong
                    AssertEqual(r2_r_rr.questions(), (uint32_t)3);
                    AssertEqual(r2_r_rr.score(), (uint32_t)2);
                    AssertEqual(r2_r_rr.max(), (uint32_t)2);

                // round 3 (indented for clarity)
                    Message r3_q_msg(conn.read());
                    AssertEqual(r3_q_msg.type(), 'u');

                    Question r3_q(r3_q_msg.body().content());
                    AssertEqual(r3_q, q3.getQuestion());

                    // send response
                    conn.write(Message('u', std::string(1, (char)(q3.getSolution()+1))).serialize());
                    
                    // read result
                    Message r3_r_msg(conn.read());
                    AssertEqual(r3_r_msg.type(), 'u');
                    
                    // round results
                    RoundResults r3_r_rr;
                    AssertNotExcept(r3_r_rr = RoundResults::deserialize(r3_r_msg.body().content()));
                    AssertEqual(r3_r_rr.correct(), false);
                    AssertEqual(r3_r_rr.ratio(), .5); // i wrong right, the other got it right
                    AssertEqual(r3_r_rr.questions(), (uint32_t)3);
                    AssertEqual(r3_r_rr.score(), (uint32_t)2);
                    AssertEqual(r3_r_rr.max(), (uint32_t)2);

                // wait for termination message or next question (in this case, termination)
                    Message t_msg(conn.read());
                    AssertEqual(t_msg.type(), 't');

                    conn.write(Message('o', "").serialize()); // ack
            } catch(const std::exception& e) {
                std::cout << "Exception in client thread 1 : " << e.what() << std::endl;
            }
        });

        std::thread ct2([&](){
            try 
            {
                { // wait for host to be ready
                    std::unique_lock<std::mutex> l(host_mutex);
                    while (!hostReady) cv.wait(l);
                } // host is ready

                TcpSocket conn;
                conn.Connect(sessionHost);
                conn.write(Message('n', Body("contestant\n" + nickname2 + "\n")).serialize());
                Message res(conn.read());
                
                AssertEqual(res.type(), 'o'); // the response should be OK

                // logged in ... now wait for question

                // round 1 (indented for clarity)
                    Message r1_q_msg(conn.read());
                    AssertEqual(r1_q_msg.type(), 'u');

                    Question r1_q(r1_q_msg.body().content());
                    AssertEqual(r1_q, q1.getQuestion());

                    // send response
                    conn.write(Message('u', std::string(1, q1.getSolution())).serialize());
                    
                    // read result
                    Message r1_r_msg(conn.read());
                    AssertEqual(r1_r_msg.type(), 'u');
                    
                    // round results
                    RoundResults r1_r_rr;
                    AssertNotExcept(r1_r_rr = RoundResults::deserialize(r1_r_msg.body().content()));
                    AssertEqual(r1_r_rr.correct(), true);
                    AssertEqual(r1_r_rr.ratio(), 1.);
                    AssertEqual(r1_r_rr.questions(), (uint32_t)3);
                    AssertEqual(r1_r_rr.score(), (uint32_t)1);
                    AssertEqual(r1_r_rr.max(), (uint32_t)1);

                // round 2 (indented for clarity)
                    Message r2_q_msg(conn.read());
                    AssertEqual(r2_q_msg.type(), 'u');

                    Question r2_q(r2_q_msg.body().content());
                    AssertEqual(r2_q, q2.getQuestion());

                    // send response
                    conn.write(Message('u', std::string(1, (char)(q3.getSolution()+1))).serialize());
                    
                    // read result
                    Message r2_r_msg(conn.read());
                    AssertEqual(r2_r_msg.type(), 'u');
                    
                    // round results
                    RoundResults r2_r_rr;
                    AssertNotExcept(r2_r_rr = RoundResults::deserialize(r2_r_msg.body().content()));
                    AssertEqual(r2_r_rr.correct(), false);
                    AssertEqual(r2_r_rr.ratio(), .5);
                    AssertEqual(r2_r_rr.questions(), (uint32_t)3);
                    AssertEqual(r2_r_rr.score(), (uint32_t)1);
                    AssertEqual(r2_r_rr.max(), (uint32_t)2);

                // round 3 (indented for clarity)
                    Message r3_q_msg(conn.read());
                    AssertEqual(r3_q_msg.type(), 'u');

                    Question r3_q(r3_q_msg.body().content());
                    AssertEqual(r3_q, q3.getQuestion());

                    // send response
                    conn.write(Message('u', std::string(1, q3.getSolution())).serialize());
                    
                    // read result
                    Message r3_r_msg(conn.read());
                    AssertEqual(r3_r_msg.type(), 'u');
                    
                    // round results
                    RoundResults r3_r_rr;
                    AssertNotExcept(r3_r_rr = RoundResults::deserialize(r3_r_msg.body().content()));
                    AssertEqual(r3_r_rr.correct(), true);
                    AssertEqual(r3_r_rr.ratio(), .5);
                    AssertEqual(r3_r_rr.questions(), (uint32_t)3);
                    AssertEqual(r3_r_rr.score(), (uint32_t)2);
                    AssertEqual(r3_r_rr.max(), (uint32_t)2);

                // wait for termination message or next question (in this case, termination)
                    Message t_msg(conn.read());
                    AssertEqual(t_msg.type(), 't');

                    conn.write(Message('o', "").serialize()); // ack
            } catch(const std::exception& e) {
                std::cout << "Exception in client thread 2 : " << e.what() << std::endl;
            }
        });

        // join all threads
        if (ct1.joinable()) ct1.join();
        if (ct2.joinable()) ct2.join();
        if (cs.joinable()) cs.join();
    }

    ENDCASE,

    CASE("playRound_30Contestants8Rounds") // abusing timing issues
        // SUMMARY
        //      GIVEN A contest session on any given port with 8 questions
        //      WHEN  Join 30 contestants before timeout
        //      THEN  The 30 contestants can play for 8 rounds round

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        qs.emplace(q2_Id, q2);
        qs.emplace(q3_Id, q3);
        qs.emplace(q4_Id, q4);
        qs.emplace(q5_Id, q5);
        qs.emplace(q6_Id, q6);
        qs.emplace(q7_Id, q7);
        qs.emplace(q8_Id, q8);
        
        size_t timeout = 3;
        std::vector<std::string> names = ____name_set;
        std::sort(names.begin(), names.end());
        uint32_t contestants = (uint32_t)names.size();

        std::mutex round_correct_mutex;
        std::map<uint32_t, uint32_t> round_correct;
        round_correct.emplace(q1_Id, 0);
        round_correct.emplace(q2_Id, 0);
        round_correct.emplace(q3_Id, 0);
        round_correct.emplace(q4_Id, 0);
        round_correct.emplace(q5_Id, 0);
        round_correct.emplace(q6_Id, 0);
        round_correct.emplace(q7_Id, 0);
        round_correct.emplace(q8_Id, 0);

        uint32_t max = 0;
        std::vector<uint32_t> scores;
        for(size_t i = 0; i < contestants; ++i)
            scores.push_back(0);

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::thread cs([&](){
            try {
                ContestSession cs(qs);
                { // announce host ready
                    std::unique_lock<std::mutex> l(host_mutex);
                    sessionHost = cs.getSocket().local();
                    hostReady = true;
                } cv.notify_all(); // session host is ready
                
                AssertEqual(cs.getState(), READY);
                cs.StartSession(timeout);
                AssertEqual(cs.getState(), RUNNING);

                std::vector<std::string> namesRead = cs.getNames();
                std::sort(namesRead.begin(), namesRead.end());
                AssertEqual(namesRead.size(), (size_t)contestants);
                AssertEqual(namesRead, names);
                
                for(auto& q : qs) cs.PlayRound(q.first);

                cs.TerminateSession();

                AssertEqual(cs.getState(), TERMINATED);

                ContestStats stats = cs.getStats();
                //AssertEqual(stats.average(), overallAvgTotal/contestants); // TODO CALCULATE
                AssertEqual(stats.contestants(), (uint32_t)contestants);
                AssertEqual((uint32_t)stats.questions(), (uint32_t)qs.size());
                //AssertEqual(stats.max(), (uint32_t)overallMax);
                AssertEqual(stats.highest(), max);
            } catch(const std::exception& e) {
                std::cout << "Exception in contest session thread : " << e.what() << std::endl;
            }
        });

        std::vector<std::thread> contestant_threads;
        for(size_t idx = 0; idx < contestants; idx++)
            contestant_threads.push_back(std::thread([&](size_t i){
                try 
                {
                    { // wait for host to be ready
                        std::unique_lock<std::mutex> l(host_mutex);
                        while (!hostReady) cv.wait(l);
                    } // host is ready
                    TcpSocket conn;
                    conn.Connect(sessionHost);
                    conn.write(Message('n', Body("contestant\n" + names[i] + "\n")).serialize());
                    Message res(conn.read());
                    AssertEqual(res.type(), 'o'); // the response should be OK
                    // logged in ... now wait for question
                    for(size_t round = 1; round <= qs.size(); ++round)
                    {
                        // read the question's message
                        Message q_msg(conn.read());
                        AssertEqual(q_msg.type(), 'u');
                        // question deserialized
                        Question q(q_msg.body().content());
                        AssertEqual(q, qs.at((uint32_t)round).getQuestion());
                        // respond randomly right or wrong
                        int wrong = UniformRandom(0,1).generate(); // 0 not wrong, 1 wrong
                        
                        if(!wrong) 
                        {
                            std::unique_lock<std::mutex> l(round_correct_mutex);
                            scores[(uint32_t)i]++;
                            round_correct[(uint32_t)round]++;
                            max = *(std::max_element(scores.begin(), scores.end()));
                        }

                        conn.write(Message('u', std::string(1, char(qs.at((uint32_t)round).getSolution()+wrong))).serialize());
                        // read round response
                        Message res(conn.read());
                        AssertEqual(res.type(), 'u');
                        // round results
                        RoundResults rr;
                        AssertNotExcept(rr = RoundResults::deserialize(res.body().content()));
                        AssertEqual(rr.correct(), (!wrong));
                        AssertEqual(rr.ratio(), double((double)round_correct[(uint32_t)round] / (double)contestants));
                        AssertEqual(rr.questions(), (uint32_t)qs.size());
                        AssertEqual(rr.score(), scores[(uint32_t)i]);
                        AssertEqual(rr.max(), (uint32_t)max);
                    }

                    // wait for termination message or next question (in this case, termination)
                    Message t_msg(conn.read());
                    AssertEqual(t_msg.type(), 't');

                    conn.write(Message('o', "").serialize()); // ack
                } catch(const std::exception& e) {
                    std::cout << "Exception in client thread " << i << " : " << e.what() << std::endl;
                }
            }, idx));


        for(auto& c_th : contestant_threads)
            if(c_th.joinable()) c_th.join();
        if (cs.joinable()) cs.join();

    ENDCASE,

    CASE("playRound_30Contestants8RoundsRandTiming") // abusing timing issues
        // SUMMARY
        //      GIVEN A contest session on any given port with 8 questions
        //      WHEN  Join 30 contestants before timeout
        //      THEN  The 30 contestants can play for 8 rounds round

        std::map<uint32_t, SolvedQuestion> qs;
        qs.emplace(q1_Id, q1);
        qs.emplace(q2_Id, q2);
        qs.emplace(q3_Id, q3);
        qs.emplace(q4_Id, q4);
        qs.emplace(q5_Id, q5);
        qs.emplace(q6_Id, q6);
        qs.emplace(q7_Id, q7);
        qs.emplace(q8_Id, q8);
        
        size_t timeout = 20;
        std::vector<std::string> names = ____name_set;
        std::sort(names.begin(), names.end());
        uint32_t contestants = (uint32_t)names.size();

        std::mutex round_correct_mutex;
        std::map<uint32_t, uint32_t> round_correct;
        round_correct.emplace(q1_Id, 0);
        round_correct.emplace(q2_Id, 0);
        round_correct.emplace(q3_Id, 0);
        round_correct.emplace(q4_Id, 0);
        round_correct.emplace(q5_Id, 0);
        round_correct.emplace(q6_Id, 0);
        round_correct.emplace(q7_Id, 0);
        round_correct.emplace(q8_Id, 0);

        uint32_t max = 0;
        std::vector<uint32_t> scores;
        for(size_t i = 0; i < contestants; ++i)
            scores.push_back(0);

        std::mutex host_mutex;
        std::condition_variable cv;
        bool hostReady = false;
        Host sessionHost;

        std::thread cs([&](){
            try {
                ContestSession cs(qs);
                { // announce host ready
                    std::unique_lock<std::mutex> l(host_mutex);
                    sessionHost = cs.getSocket().local();
                    hostReady = true;
                } cv.notify_all(); // session host is ready
                
                AssertEqual(cs.getState(), READY);
                cs.StartSession(timeout);
                AssertEqual(cs.getState(), RUNNING);

                std::vector<std::string> namesRead = cs.getNames();
                std::sort(namesRead.begin(), namesRead.end());
                AssertEqual(namesRead.size(), (size_t)contestants);
                AssertEqual(namesRead, names);
                
                for(auto& q : qs) 
                    cs.PlayRound(q.first);

                cs.TerminateSession();

                AssertEqual(cs.getState(), TERMINATED);

                ContestStats stats = cs.getStats();
                //AssertEqual(stats.average(), overallAvgTotal/contestants); // TODO CALCULATE
                AssertEqual(stats.contestants(), (uint32_t)contestants);
                AssertEqual((uint32_t)stats.questions(), (uint32_t)qs.size());
                //AssertEqual(stats.max(), (uint32_t)overallMax);
                AssertEqual(stats.highest(), max);
            } catch(const std::exception& e) {
                std::cout << "Exception in contest session thread : " << e.what() << std::endl;
            }
        });

        std::vector<std::thread> contestant_threads;
        for(size_t idx = 0; idx < contestants; idx++)
            contestant_threads.push_back(std::thread([&](size_t i){
                try 
                {
                    { // wait for host to be ready
                        std::unique_lock<std::mutex> l(host_mutex);
                        while (!hostReady) cv.wait(l);
                    } // host is ready
                    TcpSocket conn;
                    // each thread waits a random amount of time before joining the game
                    int delay = UniformRandom(1,(int)timeout - 5).generate();
                    std::cout << "Contestant " << i << "(" << names[i] << ")" << " connecting in " << delay << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(delay)); 
                    std::cout << "Contestant " << i << "(" << names[i] << ")" << " will connect now " << std::endl;
                    conn.Connect(sessionHost);
                    conn.write(Message('n', Body("contestant\n" + names[i] + "\n")).serialize());
                    Message res(conn.read());
                    AssertEqual(res.type(), 'o'); // the response should be OK
                    std::cout << "Contestant " << i << "(" << names[i] << ")" << " joined! " << std::endl;

                    // logged in ... now wait for question
                    for(size_t round = 1; round <= qs.size(); ++round)
                    {
                        // read the question's message
                        Message q_msg(conn.read());
                        AssertEqual(q_msg.type(), 'u');
                        // question deserialized
                        Question q(q_msg.body().content());
                        AssertEqual(q, qs.at((uint32_t)round).getQuestion());
                        // respond randomly right or wrong
                        int wrong = UniformRandom(0,1).generate(); // 0 not wrong, 1 wrong
                        
                        if(!wrong) 
                        {
                            std::unique_lock<std::mutex> l(round_correct_mutex);
                            scores[(uint32_t)i]++;
                            round_correct[(uint32_t)round]++;
                            max = *(std::max_element(scores.begin(), scores.end()));
                        }

                        //wait random time before responding (1...5 seconds)
                        int question_delay = UniformRandom(1,(int)5).generate();
                        std::cout << "Contestant " << i << "(" << names[i] << ")" << " will answer round " << round << " in " << question_delay << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(question_delay));
                        std::cout << "Contestant " << i << "(" << names[i] << ")" << " answered round " << round << std::endl;

                        conn.write(Message('u', std::string(1, char(qs.at((uint32_t)round).getSolution()+wrong))).serialize());
                        // read round response
                        Message res(conn.read());
                        AssertEqual(res.type(), 'u');
                        // round results
                        RoundResults rr;
                        AssertNotExcept(rr = RoundResults::deserialize(res.body().content()));
                        AssertEqual(rr.correct(), (!wrong));
                        AssertEqual(rr.ratio(), double((double)round_correct[(uint32_t)round] / (double)contestants));
                        AssertEqual(rr.questions(), (uint32_t)qs.size());
                        AssertEqual(rr.score(), scores[(uint32_t)i]);
                        AssertEqual(rr.max(), (uint32_t)max);
                    }

                    // wait for termination message or next question (in this case, termination)
                    Message t_msg(conn.read());
                    AssertEqual(t_msg.type(), 't');
                    conn.write(Message('o', "").serialize());
                } catch(const std::exception& e) {
                    std::cout << "Exception in client thread " << i << " : " << e.what() << std::endl;
                }
            }, idx));


        for(auto& c_th : contestant_threads)
            if(c_th.joinable()) c_th.join();
        if (cs.joinable()) cs.join();

    ENDCASE

ENDTEST

RUNTEST