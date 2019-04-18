#ifndef __CONTESTMEISTER_CLIENT_H__
#define __CONTESTMEISTER_CLIENT_H__

#include "Host.h"
#include "Contestant.h"
#include "RoundResults.h"
#include "Except.h"
#include "Message.h"
#include "Question.h"
#include "SolvedQuestion.h"

#include <string>
#include <ostream>
#include <iostream>
#include <functional>

using client_handler = std::function<void (std::string)>;
using client_handler_set = std::map<char, client_handler>;

class ContestmeisterClient
{
    public :

        ContestmeisterClient(Host host, std::istream& is = std::cin, 
                                        std::ostream& os = std::cout,
                                        std::ostream& es = std::cerr);

        void run(void);

    protected :

        Host _host;        // session's host
        std::istream& _is; // input  stream
        std::ostream& _os; // output stream
        std::ostream& _es; // error  stream
        bool _running = false;

        TcpSocket _sock;

        client_handler_set _handlers;

        const std::string _menu = 
            "/----------------------------------------------\\\n"
            "| Option            | Details                  |\n"
            "------------------------------------------------\n"
            "| p <qid>           | input question with id   |\n"
            "| d <qid>           | delete question by id    |\n"
            "| g <qid>           | get question by id       |\n"
            "|-------------------|--------------------------|\n"
            "| s <cid>           | set contest by id        |\n"
            "| a <cid> <qid>     | add question to contest  |\n"
            "| b <cid>           | begin contest            |\n"
            "| l                 | list contests            |\n"
            "| r <cid>           | review contest by id     |\n"
            "|-------------------|--------------------------|\n"
            "| k                 | kill server              |\n"
            "| q                 | quit                     |\n"
            "| h                 | to display this menu     |\n"
           "\\----------------------------------------------/\n";

        void prompt_loop(void);
        bool connect(void);
        void init_handlers(void);
};

#endif // __CONTESTMEISTER_CLIENT_H__