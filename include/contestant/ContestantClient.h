#ifndef __CONTESTANT_CLIENT_H__
#define __CONTESTANT_CLIENT_H__

#include "Host.h"
#include "Contestant.h"
#include "RoundResults.h"
#include "Except.h"
#include "Message.h"
#include "Question.h"

#include <string>
#include <ostream>
#include <iostream>

class ContestantClient
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

        bool        joinContest(void);
        void        playAllRounds(void);
        void        playRound(Message round_msg, uint32_t round);
};

#endif // __CONTESTANT_CLIENT_H_