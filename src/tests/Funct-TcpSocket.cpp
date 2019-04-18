#include "TcpSocket.h"
#include "Test.h"
#include "UniformRandom.h"

#include <string>
#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

TEST

    CASE("socketNormalBipartiteOnce")
        // synchronization
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;

        // result variables
        ssize_t bytesWrtByClient;
        std::string msgReadByClient;
        ssize_t bytesWrtByServer;
        std::string msgReadByServer;

        // common data
        Host h("localhost", "6666");
        std::string msgFromClient("Hello server! This is client."),
                    msgFromServer("Hey there client! This is server.");

        // threads
        std::thread client([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;
            
            l.lock(); while (!ready) cv.wait(l); l.unlock();

            s.Connect(h);
            bytesWrtByClient = s.write(msgFromClient);
            msgReadByClient = s.read();
        });
        
        std::thread server([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;

            s.Bind(h);
            s.Listen();

            // let client know im ready
            l.lock();
            ready = true; 
            l.unlock();
            cv.notify_all();
            
            // will return connection socket
            TcpSocket cs = s.Accept();

            msgReadByServer = cs.read();
            bytesWrtByServer = cs.write(msgFromServer);
        });

        server.join();
        client.join();

        // assert
        AssertEqual(msgReadByClient, msgFromServer);
        AssertEqual(msgReadByServer, msgFromClient);
        AssertEqual((size_t)bytesWrtByClient, msgFromClient.size());
        AssertEqual((size_t)bytesWrtByServer, msgFromServer.size());
    ENDCASE,

    CASE("socketHugeDataExchangeOnce")
        // common data
        Host h("localhost", "110812");

        std::string msgFromClient = "Hey server! Look, I'll "
                                    "send a lot of garbage:\n",
                    msgFromServer = "That was some heavy garbage...\n"
                                    "Here, have some too:\n";

        UniformRandom<uint8_t> uf(97,122);
        for (size_t i = 0; i < (100*__BUFWR); ++i)
        {
            msgFromClient.push_back((char)uf.generate());
            msgFromServer.push_back((char)uf.generate());
        }

        msgFromClient.append("\n.\n");
        msgFromServer.append("\n.\n");

        // synchronization
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;

        // result variables
        ssize_t bytesWrtByClient;
        std::string msgReadByClient;
        ssize_t bytesWrtByServer;
        std::string msgReadByServer;

        // threads
        std::thread client([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;
            
            l.lock(); while (!ready) cv.wait(l); l.unlock();

            s.Connect(h);
            bytesWrtByClient = s.write(msgFromClient);
            msgReadByClient = s.read();
        });
        
        std::thread server([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;

            s.Bind(h);
            s.Listen();

            // let client know im ready
            l.lock();
            ready = true; 
            l.unlock();
            cv.notify_all();
            
            // will return connection socket
            TcpSocket cs = s.Accept();

            msgReadByServer = cs.read();
            bytesWrtByServer = cs.write(msgFromServer);
        });

        server.join();
        client.join();

        // assert
        AssertEqual(msgReadByClient, msgFromServer);
        AssertEqual(msgReadByServer, msgFromClient);
        AssertEqual((size_t)bytesWrtByClient, msgFromClient.size());
        AssertEqual((size_t)bytesWrtByServer, msgFromServer.size());
    ENDCASE,

    CASE("socketNormalRandomBinding")
        // synchronization
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;
        Host hToConnect;

        // result variables
        ssize_t bytesWrtByClient;
        std::string msgReadByClient;
        ssize_t bytesWrtByServer;
        std::string msgReadByServer;

        // common data
        Host h("0.0.0.0", "0");
        std::string msgFromClient("Hello server! This is client."),
                    msgFromServer("Hey there client! This is server.");
        
        // threads
        std::thread client([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;
            
            l.lock(); while (!ready) cv.wait(l); l.unlock();

            s.Connect(hToConnect);
            bytesWrtByClient = s.write(msgFromClient);
            msgReadByClient = s.read();
        });
        
        std::thread server([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            
            TcpSocket s;

            s.Bind(h);
            s.Listen();

            // let client know im ready
            l.lock();
                ready = true;
                hToConnect = s.local();
            l.unlock();
            cv.notify_all();
            
            // will return connection socket
            TcpSocket cs = s.Accept();

            msgReadByServer = cs.read();
            bytesWrtByServer = cs.write(msgFromServer);
        });

        server.join();
        client.join();
 
        // assert
        AssertEqual(msgReadByClient, msgFromServer);
        AssertEqual(msgReadByServer, msgFromClient);
        AssertEqual((size_t)bytesWrtByClient, msgFromClient.size());
        AssertEqual((size_t)bytesWrtByServer, msgFromServer.size());
    ENDCASE

ENDTEST

RUNTEST