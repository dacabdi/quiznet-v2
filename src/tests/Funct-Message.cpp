#include "TcpSocket.h"
#include "Test.h"
#include "UniformRandom.h"
#include "Message.h"
#include "ProtoError.h"

#include <string>
#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

TEST

    CASE("sendMessageBipartite")
        // setup
        bool r = true;

        // synchronization
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;

        Message sentByClient('a', "Hi this is my payload!");
        Message sentByServer('b', "Client, you sent me a type 'a' request");
        Message receivedByClient;
        Message receivedByServer;

        // common data
        Host h("0.0.0.0", "3360");

        // threads
        std::thread client([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;
            
            l.lock(); while (!ready) cv.wait(l); l.unlock();

            s.Connect(h);
            s.write(sentByClient.serialize());
            receivedByClient = s.read();
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

            receivedByServer = Message::deserialize(cs.read());
            cs.write(sentByServer.serialize());
        });

        server.join();
        client.join();

        // assert
        r = assertEqual(sentByClient, receivedByServer) && r;
        r = assertEqual(sentByServer, receivedByClient) && r;

        return r;
    ENDCASE,
    
    CASE("sendHugeMessageBipartite")
        // setup
        bool r = true;

        // synchronization
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;

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

        Message sentByClient('a', msgFromClient);
        Message sentByServer('b', msgFromServer);
        Message receivedByClient;
        Message receivedByServer;

        // common data
        Host h("0.0.0.0", "3360");

        // threads
        std::thread client([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;
            
            l.lock(); while (!ready) cv.wait(l); l.unlock();

            s.Connect(h);
            s.write(sentByClient.serialize());
            receivedByClient = s.read();
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

            receivedByServer = Message::deserialize(cs.read());
            cs.write(sentByServer.serialize());
        });

        server.join();
        client.join();

        // assert
        r = assertEqual(sentByClient, receivedByServer) && r;
        r = assertEqual(sentByServer, receivedByClient) && r;

        return r;
    ENDCASE,

    CASE("sendAnErrorMessage")
        // setup
        bool r = true;

        // synchronization
        std::mutex m;
        std::condition_variable cv;
        bool ready = false;

        ProtoError eSentByServer(CTNFND, "Nah, I can't!");
        ProtoError eReceivedByClient;

        Message sentByClient('a', "Hey, can you find Waldo?!");
        Message sentByServer('e', eSentByServer.serialize());

        Message receivedByClient;
        Message receivedByServer;

        // common data
        Host h("0.0.0.0", "3360");

        // threads
        std::thread client([&]()
        {
            std::unique_lock<std::mutex> l(m, std::defer_lock);
            TcpSocket s;
            
            l.lock(); while (!ready) cv.wait(l); l.unlock();

            s.Connect(h);
            s.write(sentByClient.serialize());
            receivedByClient = s.read();
            eReceivedByClient = ProtoError::deserialize(receivedByClient.body().content());
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

            receivedByServer = Message::deserialize(cs.read());
            cs.write(sentByServer.serialize());
        });

        server.join();
        client.join();

        // assert
        r = assertEqual(sentByClient, receivedByServer) && r;
        r = assertEqual(sentByServer, receivedByClient) && r;
        r = assertEqual(eReceivedByClient, eSentByServer) && r;

        return r;
    ENDCASE

ENDTEST

RUNTEST