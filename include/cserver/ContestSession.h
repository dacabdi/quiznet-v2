#ifndef __CONTEST_SESSION_H__
#define __CONTEST_SESSION_H__

#include "SolvedQuestion.h"
#include "TcpSocket.h"
#include "Player.h"

#include <map>
#include <mutex>
#include <thread>

/*
    Each ContestSession object represents an active session of a contest.
    The constructor takes in a reference to the map of questions held by
    the Contest object and a TcpSocket already bound to an address.

    The ContestSession object will start accepting connections for one minute
    setup the sessions, and finally run the contest one question at a time.
    Once the contest is over, the session will terminate. 
*/

class ContestSession
{
    public:

        ContestSession(TcpSocket& sock, std::map<uint32_t, SolvedQuestion>&);

    protected:

        TcpSocket& _sock;
        std::map<uint32_t, SolvedQuestion>& _questionsRef;
        size_t _rounds;

        std::map<std::string, Player> _players;
        std::mutex _playersMutex;

        void readPlayers(void);
};

#endif // __CONTEST_SESSION_H__
