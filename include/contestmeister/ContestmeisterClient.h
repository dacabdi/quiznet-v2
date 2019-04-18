#ifndef __CONTESTMEISTER_CLIENT_H__
#define __CONTESTMEISTER_CLIENT_H__

#include "Host.h"
#include "Contestant.h"
#include "RoundResults.h"
#include "Except.h"
#include "Message.h"
#include "Question.h"

#include <string>
#include <ostream>
#include <iostream>

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

        TcpSocket _sock;

        const std::string _menu = 
            "/----------------------------------------------\\\n"
            "| Option            | Details                  |\n"
            "------------------------------------------------\n"
            "| p                 | input question           |\n"
            "| r                 | present random question  |\n"
            "| d <id>            | delete question by id    |\n"
            "| g <id>            | get question by id       |\n"
            "| c <id> <choice>   | check ans                |\n"
            "| k                 | kill server              |\n"
            "| q                 | quit                     |\n"
            "| h                 | to display this menu     |\n"
            "\\----------------------------------------------/\n"
        ;

        void promptLoop(void);
        bool connect(void);
};

ContestmeisterClient::ContestmeisterClient(Host host, 
                            std::istream& is = std::cin, 
                            std::ostream& os = std::cout,
                            std::ostream& es = std::cerr)
: _host(host), _is(is), _os(os), _es(es)
{}

void ContestmeisterClient::promptLoop(void)
{
    _os << _menu;

    char option = '\0';
    std::string prompt = "> ";
    std::string invalidMsg = "Invalid option, please try again.";

    bool running = true;
    while(running)
    {
        _os << prompt << std::flush;
        
        std::string line = "";
        std::getline(_is, line);
        
        size_t pos = line.find_first_not_of(" \t\n\r");
        if(pos == std::string::npos)
            continue;

        line = line.substr(pos);
        option = line.at(0);

        if (option == 'h') 
        {
            _os << _menu;
            continue;
        } else if(!_prepareRequest.count(option))
            _es << invalidMsg << std::endl;
        else // options that require requests
            doRequest(option, line);
    }
}

bool ContestmeisterClient::connect(void)
{
    Message res;
    try {
        _sock.Connect(_host);
        _sock.write(Message('n',"contestmeister\n").serialize());
        std::string res_str = _sock.read();
        res = Message(res_str);
    } catch (const Except& e) {
        _es << "Failed to connect to server: " 
            << e.what() << std::endl;
        return false;
    }

    return res.type() == 'o';
}

void doRequest(char option, const std::string&)
{

}

#endif // __CONTESTMEISTER_CLIENT_H__