#include "Socket.h"
#include "Host.h"
#include "UniformRandom.h"
#include <iostream>
#include <ostream>
#include <istream>
#include <sstream>
#include <sys/wait.h>
#include <chrono>
#include <thread>

bool socketOpenAndClose(void);
bool hostCreate(void);
bool hostCreateOnUnavailableHost(void);
bool socketBindLocalhost(void);
bool testCommunication(void);
bool testCommunicationMultipleRW(void);
bool testCommunicationMultipleRWBiggerThanBuffer(void);
bool testCommunicationMultipleRWBiggerThanBufferAbusive(void);

void runTest(const std::string& name, bool test(void));

int main(void)
{
    runTest("socketOpenAndClose", socketOpenAndClose);
    runTest("hostCreate", hostCreate);
    runTest("hostCreateOnUnavailableHost", hostCreateOnUnavailableHost);
    runTest("socketBindLocalhost", socketBindLocalhost);
    runTest("testCommunication", testCommunication);
    runTest("testCommunicationMultipleRW", testCommunicationMultipleRW);
    runTest("testCommunicationMultipleRWBiggerThanBuffer",
        testCommunicationMultipleRWBiggerThanBuffer);
    runTest("testCommunicationMultipleRWBiggerThanBufferAbusive",
        testCommunicationMultipleRWBiggerThanBufferAbusive);

    return 0; 
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":" << std::flush;
    std::cout << (test() ? "[OK]" : "[FAILED]") << std::flush;
    std::cout << std::endl;
}

bool socketOpenAndClose(void)
{
    Socket socket(IPv4, StreamSocket, TCP);

    if(socket.getAddressDomain() != IPv4)
        return false;

    if(socket.getSocketType() != StreamSocket)
        return false;

    if(socket.getProtocol() != TCP)
        return false;

    return true;
}

bool hostCreate(void)
{
    Host host("localhost", "80");

    if(host.getAddress() != "127.0.0.1")
        return false;

    if(host.getNode() != "localhost")
        return false;

    if(host.getService() != "80")
        return false;
    
    if(host.isPassive() == true)
        return false;    
    
    return true;
}

bool hostCreateOnUnavailableHost(void)
{
    try
    {
        Host host("---icannotpossibleexist---", "666");
        return false;
    }
    catch(Exception& e)
    {}

    return true;
}

bool socketBindLocalhost(void)
{
    Socket socket(IPv4, StreamSocket, TCP);
    Host host("8080");

    socket.bindSocket(host);

    if(!socket.isBinded())
        return false;   

    socket.closeSocket();

    if(socket.isBinded())
        return false;

    return true;
}

bool testCommunication(void)
{
    std::string clientToServer = "Hello server! This is client.",
                serverToClient = "Hi there client! I am server.";
    ssize_t clientToServerLength = clientToServer.length(),
            serverToClientLength = serverToClient.length();

    bool result = true;

    // create pipe for process coordination
    int p[2]; // p[0] < reads, p[1] < writes
    int r = pipe(p); 

    if(r < 0)
        throw Exception("Failed to create pipes", "testCommunication()");
    
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process (client)

        // setup client
        Host host("localhost", "8080");
        Socket socket;

        // setup connection handler
        socket.onOutgoingConnection = [&](
            ISocket& socket, 
            const IHost& host, 
            ISocket* context)
        {
            // send a message to the server
            std::istringstream is(clientToServer);
            ssize_t writtenbytes = socket.writeToSocket(is);
            if(writtenbytes != clientToServerLength)
                result = false;

            // read server's reply
            std::ostringstream os;
            socket.readFromSocket(os);
            std::string readmsg(os.str());
            if(readmsg != serverToClient)
                result = false;
        };

        // wait for server ready message
        char buffer[6];
        int r = read(p[0], buffer, 5);
        if(r < 0)
            throw Exception(
                "Child process (client) failed to " 
                "read pipe for ready to accept message", 
                "testCommunication()");

        // if server ready, sleep for some milliseconds, and then connect
        if(std::string(buffer) == "ready")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            socket.connectTo(host);
        }
        else
            result = false;

        // if everything went as expected, tell server's process
        std::string resultMsg = (result ? "ok" : "failed");
        r = write(p[1], resultMsg.c_str(), resultMsg.length());
        if(r < 0)
            throw Exception(
                "Child process (client) failed to"
                " write to pipe final status message", 
                "testCommunication()");

        exit(EXIT_SUCCESS);
        
    }
    else if (pid > 0)
    {
        // parent process (server)

        // setup server
        Host host("8080");
        Socket socket;

        // set handler
        socket.onIncomingConnection = [&](
            ISocket& socket, 
            const IHost& host,
            ISocket* context)
        {
            // read clients's message
            std::ostringstream os;
            socket.readFromSocket(os);
            if(os.str() != clientToServer)
                result = false;
            
            // send a reply to the client
            std::istringstream is(serverToClient);
            if(socket.writeToSocket(is) != serverToClientLength)
                result = false;
        };

        // continue setting up server
        socket.bindSocket(host);
        socket.startListening();

        // tell client process we are ready
        char ready[] = "ready";
        int r = write(p[1], ready, strlen(ready));
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to write to pipe", 
                "testCommunication()");
        
        // start accepting connections
        socket.acceptConnection();

        // ask the client how it went for him
        char finalStatus[10];
        r = read(p[0], finalStatus, 10);
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to read from pipe"
                " final status message",
                "testCommunication()");

        // if it went well for client also, hurray!
        if(std::string(finalStatus) != "ok")
            result = false;

        wait(NULL); // wait for the other process to return
    }
    else
    {
        // fork failed
        throw Exception(
            "Fork failed on communication test", 
            "testCommunication()");
    }

    return result;
}


bool testCommunicationMultipleRW(void)
{
    std::string clientToServer = "Hello server! This is client.\n"
                                 "And this time I have a really\n"
                                 "long message for you. You should\n"
                                 " read and read until the whole\n"
                                 "damn thing is on your side of the\n"
                                 "fence. I'll let you know when\n"
                                 "I'm done by passing an empty line\n"
                                 "with a period followed by and\n"
                                 "end of line.\n\nOk, this"
                                 " is it for now, I'm out.\n.\n",

                serverToClient = "Hi there client! This is server. "
                                 "This time you were considerably more"
                                 " verbose than usual, luckily I knew "
                                 "you were coming\nwith a big message and"
                                 " followed our agreement on ending the "
                                 "message with a lone period. I'll do the"
                                 "same over here. So, bye!\n.\n";

    ssize_t clientToServerLength = clientToServer.length(),
            serverToClientLength = serverToClient.length();

    bool result = true;

    // create pipe for process coordination
    int p[2]; // p[0] < reads, p[1] < writes
    int r = pipe(p); 

    if(r < 0)
        throw Exception("Failed to create pipes", "testCommunication()");
    
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process (client)

        // setup client
        Host host("localhost", "8080");
        Socket socket;

        // setup connection handler
        socket.onOutgoingConnection = [&](
            ISocket& socket, 
            const IHost& host, 
            ISocket* context)
        {
            // send a message to the server
            std::istringstream is(clientToServer);
            ssize_t writtenbytes = socket.writeToSocket(is);
            if(writtenbytes != clientToServerLength)
                result = false;

            // read server's reply
            std::ostringstream os;
            ssize_t total = 0;
            bool done = false;
            std::string readmsg;
            while(!done)
            {
                total += socket.readFromSocket(os);
                readmsg.append(os.str());
                if(readmsg.substr(readmsg.length()-3, 3) == "\n.\n")
                    done = true;
            }

            if(readmsg != serverToClient)
                result = false;
        };

        // wait for server ready message
        char buffer[6];
        int r = read(p[0], buffer, 5);
        if(r < 0)
            throw Exception(
                "Child process (client) failed to " 
                "read pipe for ready to accept message", 
                "testCommunication()");

        // if server ready, sleep for some milliseconds, and then connect
        if(std::string(buffer) == "ready")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            socket.connectTo(host);
        }
        else
            result = false;

        // if everything went as expected, tell server's process
        std::string resultMsg = (result ? "ok" : "failed");
        r = write(p[1], resultMsg.c_str(), resultMsg.length());
        if(r < 0)
            throw Exception(
                "Child process (client) failed to"
                " write to pipe final status message", 
                "testCommunication()");

        exit(EXIT_SUCCESS);
        
    }
    else if (pid > 0)
    {
        // parent process (server)

        // setup server
        Host host("8080");
        Socket socket;

        // set handler
        socket.onIncomingConnection = [&](
            ISocket& socket, 
            const IHost& host,
            ISocket* context)
        {
            // read clients's message
            std::ostringstream os;
            ssize_t total = 0;
            bool done = false;
            std::string readmsg;
            while(!done)
            {
                total += socket.readFromSocket(os);
                readmsg.append(os.str());
                if(readmsg.substr(readmsg.length()-3, 3) == "\n.\n")
                    done = true;
            }

            if(readmsg != clientToServer)
                result = false;

            // send a reply to the client
            std::istringstream is(serverToClient);
            if(socket.writeToSocket(is) != serverToClientLength)
                result = false;
        };

        // continue setting up server
        socket.bindSocket(host);
        socket.startListening();

        // tell client process we are ready
        char ready[] = "ready";
        int r = write(p[1], ready, strlen(ready));
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to write to pipe", 
                "testCommunication()");
        
        // start accepting connections
        socket.acceptConnection();

        // ask the client how it went for him
        char finalStatus[10];
        r = read(p[0], finalStatus, 10);
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to read from pipe"
                " final status message",
                "testCommunication()");

        // if it went well for client also, hurray!
        if(std::string(finalStatus) != "ok")
            result = false;

        wait(NULL); // wait for the other process to return
    }
    else
    {
        // fork failed
        throw Exception(
            "Fork failed on communication test", 
            "testCommunication()");
    }

    return result;
}

bool testCommunicationMultipleRWBiggerThanBuffer(void)
{
    bool result = true;

    std::string clientToServer = "Hey server! Look, I'll "
                                 "send a lot of garbage:\n",
                serverToClient = "That was some heavy garbage...\n"
                                 "Here, have some too:\n";
    UniformRandom<uint8_t> uf(97,122);
    for (size_t i = 0; i < (100*__DATA_BUFFER_READ); ++i)
    {
        clientToServer.push_back((char)uf.generate());
        serverToClient.push_back((char)uf.generate());
    }

    clientToServer.append("\n.\n");
    serverToClient.append("\n.\n");

    ssize_t clientToServerLength = clientToServer.length(),
            serverToClientLength = serverToClient.length();

    // create pipe for process coordination
    int p[2]; // p[0] < reads, p[1] < writes
    int r = pipe(p); 

    if(r < 0)
        throw Exception("Failed to create pipes", "testCommunication()");
    
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process (client)

        // setup client
        Host host("localhost", "8080");
        Socket socket;

        // setup connection handler
        socket.onOutgoingConnection = [&](
            ISocket& socket, 
            const IHost& host, 
            ISocket* context)
        {
            // send a message to the server
            std::istringstream is(clientToServer);
            ssize_t writtenbytes = socket.writeToSocket(is);
            if(writtenbytes != clientToServerLength)
                result = false;

            // read server's reply
            ssize_t total = 0;
            bool done = false;
            std::string readmsg = "";
            while(!done)
            {
                std::ostringstream os;
                total += socket.readFromSocket(os);
                readmsg.append(os.str());
                if(readmsg.substr(readmsg.length()-3, 3) == "\n.\n")
                    done = true;
            }

            if(readmsg != serverToClient)
                result = false;
        };

        // wait for server ready message
        char buffer[6];
        int r = read(p[0], buffer, 5);
        if(r < 0)
            throw Exception(
                "Child process (client) failed to " 
                "read pipe for ready to accept message", 
                "testCommunication()");

        // if server ready, sleep for some milliseconds, and then connect
        if(std::string(buffer) == "ready")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            socket.connectTo(host);
        }
        else
            result = false;

        // if everything went as expected, tell server's process
        std::string resultMsg = (result ? "ok" : "failed");
        r = write(p[1], resultMsg.c_str(), resultMsg.length());
        if(r < 0)
            throw Exception(
                "Child process (client) failed to"
                " write to pipe final status message", 
                "testCommunication()");

        exit(EXIT_SUCCESS);
        
    }
    else if (pid > 0)
    {
        // parent process (server)

        // setup server
        Host host("8080");
        Socket socket;

        // set handler
        socket.onIncomingConnection = [&](
            ISocket& socket, 
            const IHost& host,
            ISocket* context)
        {
            // read clients's message
            
            ssize_t total = 0;
            bool done = false;
            std::string readmsg = "";
            while(!done)
            {
                std::ostringstream os;
                total += socket.readFromSocket(os);
                readmsg.append(os.str());
                if(readmsg.substr(readmsg.length()-3, 3) == "\n.\n")
                    done = true;
            }

            if(readmsg != clientToServer)
                result = false;

            // send a reply to the client
            std::istringstream is(serverToClient);
            if(socket.writeToSocket(is) != serverToClientLength)
                result = false;
        };

        // continue setting up server
        socket.bindSocket(host);
        socket.startListening();

        // tell client process we are ready
        char ready[] = "ready";
        int r = write(p[1], ready, strlen(ready));
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to write to pipe", 
                "testCommunication()");
        
        // start accepting connections
        socket.acceptConnection();

        // ask the client how it went for him
        char finalStatus[10];
        r = read(p[0], finalStatus, 10);
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to read from pipe"
                " final status message",
                "testCommunication()");

        // if it went well for client also, hurray!
        if(std::string(finalStatus) != "ok")
            result = false;

        wait(NULL); // wait for the other process to return
    }
    else
    {
        // fork failed
        throw Exception(
            "Fork failed on communication test", 
            "testCommunication()");
    }

    return result;
}



bool testCommunicationMultipleRWBiggerThanBufferAbusive(void)
{
    bool result = true;

    std::string clientToServer = "Hey server! Look, I'll "
                                 "send a lot of garbage:\n",
                serverToClient = "That was some heavy garbage...\n"
                                 "Here, have some too:\n";
    UniformRandom<uint8_t> uf(97,122);
    for (size_t i = 0; i < (10000*__DATA_BUFFER_READ); ++i)
    {
        clientToServer.push_back((char)uf.generate());
        serverToClient.push_back((char)uf.generate());
    }

    clientToServer.append("\n.\n");
    serverToClient.append("\n.\n");

    ssize_t clientToServerLength = clientToServer.length(),
            serverToClientLength = serverToClient.length();

    // create pipe for process coordination
    int p[2]; // p[0] < reads, p[1] < writes
    int r = pipe(p); 

    if(r < 0)
        throw Exception("Failed to create pipes", "testCommunication()");
    
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process (client)

        // setup client
        Host host("localhost", "8080");
        Socket socket;

        // setup connection handler
        socket.onOutgoingConnection = [&](
            ISocket& socket, 
            const IHost& host, 
            ISocket* context)
        {
            // send a message to the server
            std::istringstream is(clientToServer);
            ssize_t writtenbytes = socket.writeToSocket(is);
            if(writtenbytes != clientToServerLength)
                result = false;

            // read server's reply
            ssize_t total = 0;
            bool done = false;
            std::string readmsg = "";
            while(!done)
            {
                std::ostringstream os;
                total += socket.readFromSocket(os);
                readmsg.append(os.str());
                if(readmsg.substr(readmsg.length()-3, 3) == "\n.\n")
                    done = true;
            }

            if(readmsg != serverToClient)
                result = false;
        };

        // wait for server ready message
        char buffer[6];
        int r = read(p[0], buffer, 5);
        if(r < 0)
            throw Exception(
                "Child process (client) failed to " 
                "read pipe for ready to accept message", 
                "testCommunication()");

        // if server ready, sleep for some milliseconds, and then connect
        if(std::string(buffer) == "ready")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            socket.connectTo(host);
        }
        else
            result = false;

        // if everything went as expected, tell server's process
        std::string resultMsg = (result ? "ok" : "failed");
        r = write(p[1], resultMsg.c_str(), resultMsg.length());
        if(r < 0)
            throw Exception(
                "Child process (client) failed to"
                " write to pipe final status message", 
                "testCommunication()");

        exit(EXIT_SUCCESS);
        
    }
    else if (pid > 0)
    {
        // parent process (server)

        // setup server
        Host host("8080");
        Socket socket;

        // set handler
        socket.onIncomingConnection = [&](
            ISocket& socket, 
            const IHost& host,
            ISocket* context)
        {
            // read clients's message
            
            ssize_t total = 0;
            bool done = false;
            std::string readmsg = "";
            while(!done)
            {
                std::ostringstream os;
                total += socket.readFromSocket(os);
                readmsg.append(os.str());
                if(readmsg.substr(readmsg.length()-3, 3) == "\n.\n")
                    done = true;
            }

            if(readmsg != clientToServer)
                result = false;

            // send a reply to the client
            std::istringstream is(serverToClient);
            if(socket.writeToSocket(is) != serverToClientLength)
                result = false;
        };

        // continue setting up server
        socket.bindSocket(host);
        socket.startListening();

        // tell client process we are ready
        char ready[] = "ready";
        int r = write(p[1], ready, strlen(ready));
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to write to pipe", 
                "testCommunication()");
        
        // start accepting connections
        socket.acceptConnection();

        // ask the client how it went for him
        char finalStatus[10];
        r = read(p[0], finalStatus, 10);
        if(r < 0)
            throw Exception(
                "Parent process (server) failed to read from pipe"
                " final status message",
                "testCommunication()");

        // if it went well for client also, hurray!
        if(std::string(finalStatus) != "ok")
            result = false;

        wait(NULL); // wait for the other process to return
    }
    else
    {
        // fork failed
        throw Exception(
            "Fork failed on communication test", 
            "testCommunication()");
    }

    return result;
}