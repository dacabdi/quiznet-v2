#ifndef __QUIZSERVER__H__
#define __QUIZSERVER__H__

#include "Exception.h"
#include "Utils.h"
#include "Error.h"
#include "Escape.h"
#include "IHost.h"
#include "Socket.h"
#include "IQuizBook.h"
#include "Request.h"

#include <thread>
#include <map>
#include <fstream>
#include <functional>
#include <string>
#include <iostream>
#include <iomanip>


class QuizServer
{
    public:
        
        QuizServer(IQuizBook* quizbook, 
                   IHost* host, 
                   ISocket* socket,
                   const std::string& filename);

        void setLogger(std::function<void (const std::string&)> logger);
        void setLoggerVerbose(bool verbose = true){_verbose = verbose;};

        void run(void);

        ~QuizServer() {};

        const static std::map<size_t, ErrorMessage> errors;

    protected:

        IHost* _host;
        IQuizBook* _quizbook;
        ISocket* _socket;
        
        std::string _filename;

        bool _running = false;
        bool _clientOn = false;  // client alive for persistent conn 

        bool _verbose = false;

        // request handlers
        std::map<const char,
        std::function<Response (const Request&)>> _handlers;
        
        std::function<void (const std::string&)> _log = 
        [&](const std::string& str) {};

        void exchange(ISocket& socket);
        void persistentSession(ISocket &socket);

        void setRequestHandlers(void);
        void onChange(IQuizBook* book);

        Response attendRequest(ISocket& socket);
        Request readRequest(ISocket& socket);
};

#endif // __QUIZSERVER__H__