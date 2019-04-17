#ifndef __CONTESTANT_CLIENT_H__
#define __CONTESTANT_CLIENT_H__

#include "Host.h"
#include "Contestant.h"
#include "RoundResults.h"
#include "Except.h"

#include <string>
#include <ostream>
#include <iostream>

/*class ContestantClient
{
    public :

        ContestantClient(Host host, std::istream& is = std::cin, 
                                    std::ostream& os = std::cout,
                                    std::ostream& es = std::cerr);

        void run(void);

    protected :

        Host _host;        // session's host
        std::istream& _is; // input  stream
        std::ostream& _os; // output stream
        std::ostream& _es; // error  stream

        Contestant _contestant; // self representation

        std::string printRoundResults(void);
        std::string printFinalStats(void);
        void        joinContest(void);
};


ContestantClient::ContestantClient(Host host, std::istream& is = std::cin, 
                                              std::ostream& os = std::cout,
                                              std::ostream& es = std::cerr)
: _host(host), _is(is), _os(os), _es(es)
{}

void ContestantClient::run(void)
{
    
}

void ContestantClient::joinContest(void)
{
    // try to connect
    TcpSocket conn;
    try {
        _os << "Connecting to a contest on " << _host.fullName() << " ... " << std::flush;
        conn.Connect(_host); _os << "OK" << std::endl;
    } catch (const Except& e) {
        if(e.Errno == ECONNREFUSED)
        _os << "[FAILED]\n" 
        _es << e.what() << std::endl;
        return;
    }




        conn.write(Message('n', Body("contestant\n" + names[i] + "\n")).serialize());
        Message res(conn.read());
        AssertEqual(res.type(), 'o'); // the response should be OK
        std::cout << "Contestant " << i << "(" << names[i] << ")" << " joined! " << std::endl;
    }
}*/

#endif // __CONTESTANT_CLIENT_H_