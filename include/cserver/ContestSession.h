#ifndef __CONTEST_SESSION_H__
#define __CONTEST_SESSION_H__

#include "SolvedQuestion.h"
#include "TcpSocket.h"
#include "Contestant.h"
#include "Message.h"

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

class ContestSession
{
    public:

        ContestSession(const std::map<uint32_t, SolvedQuestion>&);
        
        void GatherContestants(const size_t seconds = 60);
        void PlayRound(const uint32_t qId);
        
        TcpSocket& getSocket(void);
        std::vector<std::string> getNames(void) const;
        const std::map<std::string, Contestant>& getContestants(void) const;
        const std::map<uint32_t, uint32_t>& getStats(void) const;
        uint32_t getStats(const uint32_t qId) const;
        
        
    protected:

        TcpSocket                                 _sock;    // contest socket
        const std::map<uint32_t, SolvedQuestion>& _sq;      // question set reference
        std::map<uint32_t, uint32_t>              _stats;   // questions statistics
        std::map<std::string, Contestant>         _contestants; std::mutex _mutex;
        uint32_t _max = 0;
        
        // workflow methods
        std::map<std::string, Contestant> _gatherContestants(const size_t seconds);
};

#endif // __CONTEST_SESSION_H__
