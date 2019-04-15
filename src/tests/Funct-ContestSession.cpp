#include "Test.h"
#include "Utils.h"
#include "ContestSession.h"

#include <algorithm>

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
            
            cs.GatherContestants(timeout);
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
            
            cs.GatherContestants(timeout);
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
            
            cs.GatherContestants(timeout);
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

            cs.GatherContestants(timeout);
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
            cs.GatherContestants(timeout);
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
            
            cs.GatherContestants(timeout);
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

        size_t timeout = 3; // discretionary time to join all contestants
        
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

            cs.GatherContestants(timeout);
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
            cs.GatherContestants(timeout);
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
            cs.GatherContestants(timeout);
            
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
            cs.GatherContestants(timeout);
            
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

    ENDCASE

ENDTEST

RUNTEST