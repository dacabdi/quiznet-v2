#ifndef __CONTEST_SESSION_H__
#define __CONTEST_SESSION_H__

#include "SolvedQuestion.h"
#include "TcpSocket.h"
#include "Contestant.h"
#include "Message.h"
#include "RoundResults.h"
#include "ContestStats.h"
#include "Common.h"

#include <map>
#include <mutex>
#include <thread>
#include <future>
#include <sstream>

/*
    Each ContestSession object represents an active session of a contest.
    The constructor takes in a reference to the map of questions held by
    the Contest object and a TcpSocket already bound to an address.

    The ContestSession object will start accepting connections for one minute
    setup the sessions, and finally run the contest one question at a time.
    Once the contest is over, the session will terminate. 
*/

using contestants_set = std::map<std::string, Contestant>;
using questions_set   = std::map<uint32_t, SolvedQuestion>;
using scores_set      = std::map<uint32_t, uint32_t>;

enum ContestState 
{
    INVALID = 0,   // socket not binded and uninitialized
    READY = 1,     // socket bound and all data cleared
    STARTING = 2,  // contest announced, accepting and gathering contestants
    RUNNING = 3,   // contestant's gathered, can run rounds
    TERMINATED = 4 // contest was called off for all contestants
};

class ContestSession
{
    public:

        ContestSession(const questions_set&);
        
        void StartSession(const size_t seconds = 60);
        void PlayRound(const uint32_t qId);
        void TerminateSession(void);

        TcpSocket& getSocket(void);
        
        std::vector<std::string> getNames(void) const;
        const contestants_set& getContestants(void) const;
        
        ContestStats getStats(void) const;
        
        ContestState getState(void);

    protected:

        TcpSocket             _sock;    // contest socket
        const questions_set & _sq;      // question set reference
        scores_set            _stats;   // questions statistics
        contestants_set       _contestants; std::mutex _contestants_mutex;
        uint32_t              _max = 0; // max score

        ContestState _state; std::mutex _state_mutex;

        // internal helpers
        contestants_set _gatherContestants(const size_t seconds);
};

#endif // __CONTEST_SESSION_H__
