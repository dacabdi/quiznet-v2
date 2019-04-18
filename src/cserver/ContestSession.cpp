#include "ContestSession.h"

ContestSession::ContestSession(const questions_set questions)
: _sock(TcpSocket()), _sq(questions), _max((uint32_t)0), _state(INVALID)
{
    _sock.Bind();
    _sock.Listen();

    // init stats
    for (const auto &q : _sq) 
        _stats.emplace(q.first, (uint32_t)0);
        
    {
        std::unique_lock<std::mutex> l(_state_mutex);
        _state = READY;
    }
}

ContestStats ContestSession::run(const size_t timeout)
{
    StartSession(timeout);
    
    // play all rounds for all players   
    _max = 0;
    for(auto& q : _sq)
        PlayRound(q.first);

    TerminateSession();

    return getStats();
}

void ContestSession::StartSession(const size_t timeout)
{
    { // go gather contestants
        std::unique_lock<std::mutex> l(_state_mutex);
        
        if(_state != READY)
            throw Except("Attempted to start non-ready session",
                         ___WHERE);

        _state = STARTING;
    }

    _contestants = _gatherContestants(timeout);
    
    { // now that we got all the contestants, declared the contest running
        std::unique_lock<std::mutex> l(_state_mutex);
        _state = RUNNING;
    }
}

void ContestSession::PlayRound(const uint32_t qId)
{
    { // check if ready to run
        std::unique_lock<std::mutex> l(_state_mutex);
        
        if(_state != RUNNING)
            throw Except("Attempted to start run round on non-running session",
                          ___WHERE);
    }

    const SolvedQuestion& sq = _sq.at(qId);
    std::map<std::string, std::shared_future<bool>> rs; // assoc each player with future round result

    for (auto &_c : _contestants) rs.emplace(_c.first, std::async(std::launch::async, [&]() -> bool {
        Contestant& c = _c.second; // get contestant object
        try { // wrap for the possibility of the contestant leaving before the end
            std::string qStr = sq.getQuestion().serialize();
            Message msg('u', qStr);
            std::string mStr = msg.serialize();
            c.Conn.write(mStr); // send question
        } catch (const Except& e) { return false; }

        // try to get response
        Message res; try             { res = Message(c.Conn.read()); } // if contestant...
        catch (const Except& e)      { return false; }                 // ... drops dead ... 
        catch (const ProtoExcept& e) { return false; }                 // ... or provides malformed answer ... 
                                                                       // ... it will cause the contestant to lose the round
        // not the right type of message or format, also lost
        if(res.type() != 'u' || res.body().content().size() != 1) return false; 
        
        char answer = res.body().content().at(0);   // get the answer from the response

        if (sq.getSolution() == answer)
        {
            {// update score and max
                std::unique_lock<std::mutex> l(_contestants_mutex);
                _max = std::max(c.increaseScore(), _max);
            }
            _stats[qId]++; // update number of right answers for this question
            return true;
        }
        
        return false;
    }));

    for (auto &cRes : rs) cRes.second.wait(); // wait for score to be ready
    
    // do book keeping and tell contestants
    double ratio = (double) _stats[qId] / (double)_contestants.size(); // percent correct answers from all the contestants for that question
    for (auto &cRes : rs)
    {
        Contestant &c = _contestants.at(cRes.first);

        uint32_t questions = (uint32_t)_sq.size();
        uint32_t score = c.getScore();

        RoundResults rr(cRes.second.get(), ratio, questions, score, _max);
        try {
            c.Conn.write(Message('u', rr.serialize()).serialize());
            Message(c.Conn.read()); // ack results
        } 
        catch(const Except& e) {} // < ignore communication problems with the contestant
    }
}

void ContestSession::TerminateSession(void)
{
    { // check if ready to run
        std::unique_lock<std::mutex> l(_state_mutex);
        
        if(_state != RUNNING)
            throw Except("Attempted to terminate non-running session",
                          ___WHERE);
    }

    std::string bye = Message('t', "").serialize();
    for (auto& cs : _contestants)
    {
        Contestant& c = cs.second;
        try
        {
            c.Conn.write(bye);  // write bye and wait for response
            Message ack(c.Conn.read());
            if(ack.type() == 'o')
                c.Conn.Close();
        }
        catch(const Except& e) {}
        // ignore communication problem exceptions
        // if client abandons or does not ack, the session
        // should go on
    }
    _sock.Close();

    { // consider the session terminated
        std::unique_lock<std::mutex> l(_state_mutex);
        _state = TERMINATED;
    }
}

TcpSocket& ContestSession::getSocket(void)
{
    return _sock;
}

std::vector<std::string> ContestSession::getNames(void) const
{
    std::vector<std::string> names;

    for (const auto & name : _contestants)
        names.push_back(name.first);
    
    return names;
}

const std::map<std::string, Contestant>& ContestSession::getContestants(void) const
{
    return _contestants;
}

ContestStats ContestSession::getStats(void) const
{
    return ContestStats(_stats, (uint32_t)_contestants.size(), _max);
}

ContestState ContestSession::getState(void)
{
    std::unique_lock<std::mutex> l(_state_mutex);
    return _state;
}

std::map<std::string, Contestant> ContestSession::_gatherContestants(const size_t timeout)
{
    std::vector<std::thread> ths;
    std::map<std::string, Contestant> contestants;

    bool timedout = false;
    std::thread timeout_signal([&](){
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        _sock.Shutdown();
    });

    while(!timedout)
    {
        try {
            // wait for connection
            TcpSocket cs = _sock.Accept();
            // negotiate session with the contestant 
            // TODO the client has the ability to stall the session if gets stopped on negotiation
            ths.push_back(std::thread([&](TcpSocket cs) {
                while(true)
                {
                    Message req;
                    
                    // try to read request
                    try {
                        std::string msg = cs.read();
                        if (msg == "") break; // likely client dropped (socket gets closed by scope)
                        req = Message(msg); // deserialize request
                    } catch (const ProtoExcept& e) { // protocol errors, send error response
                        cs.write(Message('e', e.Error.serialize()).serialize());
                        continue;
                    } catch (const Except& e) { // network errors, disregard client
                        break;
                    }

                    // try to send response
                    try {
                        // check request type
                        if(req.type() != 'n')
                        {
                            cs.write(Message('e', ProtoError(FAILGT, "Unexpected request type").serialize()).serialize());
                            continue;
                        }

                        // get a stream to the content
                        std::istringstream iss = req.body().contentStream();
                        std::string clientType; 
                        std::getline(iss, clientType); 

                        // check that client type is "contestant"
                        if (clientType != "contestant")
                        {
                            cs.write(Message('e', ProtoError(FAILGT, "Unexpected client type: " + clientType).serialize()).serialize());
                            continue;
                        }
                            
                        std::string nickName;
                        std::getline(iss, nickName);

                        if(nickName == "")
                        {
                            cs.write(Message('e', ProtoError(EMPTYN, "").serialize()).serialize());
                            continue;
                        }

                        { // check nicknames for duplicates
                            std::unique_lock<std::mutex> l(_contestants_mutex);
                            if(contestants.count(nickName) == 0)
                            {
                                cs.write(Message('o', Body("")).serialize());
                                //if(Message(cs.read()).type() == 'o') // read ack to know contestant will be expecting questions
                                contestants.emplace(nickName, Contestant(nickName, std::move(cs)));
                                break;
                            }
                        } // end check
                        cs.write(Message('e', ProtoError(NCKDUP, nickName).serialize()).serialize());
                    } catch (const Except& e) { // network errors, disregard client
                        break;
                    }
                }
            }, std::move(cs)));
        } catch(const Except& e) {
            if     (e.Errno == EINVAL)     timedout = true;
            else if(e.Errno == ECONNRESET) continue; // client dropped dead
            else throw e;
        }
    }

    if(timeout_signal.joinable()) timeout_signal.join();

    // join all negotiating threads
    for (auto &th : ths)
        if(th.joinable())
            th.join();

    return contestants;
}