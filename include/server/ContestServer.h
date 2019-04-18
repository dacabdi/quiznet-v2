#ifndef __CONTEST_SERVER_H__
#define __CONTEST_SERVER_H__

#define ___QUESTIONS_BANK "questions.bank"
#define ___CONTESTS_BANK "contests.bank"

#include "Host.h"
#include "TcpSocket.h"
#include "QuizBook.h"
#include "Message.h"
#include "Except.h"
#include "ProtoExcept.h"
#include "Contest.h"
#include "Utils.h"

#include <mutex>
#include <thread>
#include <atomic>
#include <fstream>

#include <iostream>

using logger = std::function<void (const std::string&)>;
using handler = std::function<Message (const Message&, bool&)>;
using handler_map = std::map<const char, handler>;
using contest_set = std::map<uint32_t, Contest>;

class ContestServer
{
    public:
        
        ContestServer(QuizBook& qb, Host host);
        void SetLogger(logger);
        void run(void);

    protected:

        QuizBook& _qb;
        Host _host;
        TcpSocket _sock;

        logger _log = [&](const std::string& str){}; //do nothing by default

        handler_map _handlers;

        std::atomic<bool> _reqkill = false;

        contest_set _contests;

        // inner workings
        bool handshake(TcpSocket& conn);
        void requestLoop(TcpSocket& conn);
        void initHandlers(void);

        // helpers
        void loadContests(void);
        void writeContests(void);
        void loadQuestions(void);
        void writeQuestions(void);
};

#endif // __CONTEST_SERVER_H__