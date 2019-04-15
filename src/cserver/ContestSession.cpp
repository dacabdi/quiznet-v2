#include "ContestSession.h"

#include <iostream> //TODO delete this

ContestSession::ContestSession(const std::map<uint32_t, SolvedQuestion>& questions)
: _sock(TcpSocket()), _sq(questions)
{
    _sock.Bind();
    _sock.Listen();

    // init stats
    _max = 0;
    for (const auto &q : _sq) 
        _stats.emplace(q.first, 0.);
}

void ContestSession::GatherContestants(const size_t seconds)
{
    _contestants = _gatherContestants(seconds);
}

void ContestSession::PlayRound(const uint32_t qId)
{
    const SolvedQuestion& sq = _sq.at(qId);
    std::map<std::string, std::future<bool>> rs; // assoc each player with future round result
    for (auto &_c : _contestants) rs.emplace(_c.first, std::async([&]() -> bool {
        Contestant& c = _c.second; // get contestant object
        try { // wrap for the possibility of the contestant leaving before the end
            c.Conn.write(Message('u', sq.getQuestion().serialize()).serialize()); // send question
        } catch (const Except& e) { return false; }

        // try to get response
        Message res; try             { res = Message(c.Conn.read()); } // if contestant...
        catch (const Except& e)      { return false; }                 // ... drops dead ... 
        catch (const ProtoExcept& e) { return false; }                 // ... or provides malformed answer ... 
                                                                       // ... it will cause the contestant to lose the round
        // not the right type of message or format, also lost
        if(res.type() != 'u' || res.body().content().size() != 1) return false; 
        
        char answer = res.body().content().at(0);   // get the answer from the response
        return sq.getSolution() == answer;          // return result
    }));

    // wait for async tasks and do stats
    uint32_t correct = 0;
    for (auto &cRes : rs) 
    { 
        cRes.second.wait();              // wait for score to be ready
        if(cRes.second.get()) correct++; // and increase stats for question if correct
    } _stats[qId] = correct;
    
    // do bookkeeping and tell contestants
    double ratio = (double)correct / (double)_contestants.size();

    for (auto &cRes : rs)
    {
        Contestant &c = _contestants.at(cRes.first);
        std::string res;

        // account and prepare response
        if(cRes.second.get()) 
        {
            _max = std::max(c.increaseScore(), _max);
            res = "correct\n";
        } 
        else 
            res = "incorrect\n";

        // append
        //      1. round's correct ratio
        //      2. number of questions
        //      3. contestant's score
        //      4. max score so far
        
        res.append(std::to_string(ratio) + "\n");
        res.append(std::to_string(_sq.size()) + "\n");
        res.append(std::to_string(c.getScore()) + "\n");
        res.append(std::to_string(_max) + "\n");

        try { c.Conn.write(Message('o', res).serialize()); } 
        catch(const Except& e) {} // < ignore communication problems with the contestant
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

const std::map<uint32_t, uint32_t>& ContestSession::getStats(void) const
{
    return _stats;
}

uint32_t ContestSession::getStats(const uint32_t qId) const
{
    if(!_stats.count(qId))
        throw Except("Question stats do not exist", "ContestSession::getStats()", 
                     "Id provided" + std::to_string(qId), false);
    
    return _stats.at(qId);
}

std::map<std::string, Contestant> ContestSession::_gatherContestants(const size_t seconds)
{
    std::vector<std::thread> ths;
    std::map<std::string, Contestant> contestants;

    bool timeout = false;
    std::thread timeout_signal([&](){
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        _sock.Shutdown();
    });

    while(!timeout)
    {
        try {
            // wait for connection
            TcpSocket cs = _sock.Accept();
            // negotiate session with the contestant
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
                            cs.write(Message('e', ProtoError(FAILGT, "Empty contestant nickname").serialize()).serialize());
                            continue;
                        }

                        { // check nicknames for duplicates
                            std::unique_lock<std::mutex> l(_mutex);
                            if(contestants.count(nickName) == 0)
                            {
                                cs.write(Message('o', Body("")).serialize());
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
            if     (e.Errno == EINVAL)     timeout = true;
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




/*void ContestSession::playerThread(TcpSocket conn)
{
    ProtoError e;
    Message msg;
    char reqType;
    std::string clientType, nickname;
    uint32_t round;
    Player contestant;
    bool roundReady = false;
    
    // get nickname until valid
    do
    {
        msg = Message(conn.read());
        
        reqType = msg.type();
        std::istringstream iss = msg.body().contentStream();

        iss >> clientType;
        iss >> nickname;

        if(reqType == 'n' && clientType == "contestant")
        {
            std::unique_lock<std::mutex> l(_mutex);
            if(!_players.count(nickname))
            {
                contestant = Player(nickname);
                _players.emplace(nickname, contestant);
                break;
            }
            else e = ProtoError(NCKDUP, nickname);
        } 
        else e = ProtoError(MSGDSL, "");

        conn.write(Message('e', e.serialize()).serialize());
    } while(true);

    // send OK and number of questions
    msg = Message('o', std::to_string(_questionsRef.size()));
    conn.write(msg.serialize());

    // wait to begin
    contestant.setReady(true);
    waitOnPlayers();

    for (auto q : _questionsRef)
    {
        // put the player on non ready mode
        contestant.setReady(false);

        // send question
        msg = Message('u', q.second.getQuestion().serialize());
        conn.write(msg.serialize());

        // read response
        msg = Message(conn.read());
        char playerChoice = msg.body().content().at(0);

        std::string rBody;
        if(q.second.getSolution() == playerChoice) {
            _players[nickname].increaseScore();
            rBody = "correct";
        } else rBody = "incorrect";
        
        // put the player on ready mode and wait for the rest
        contestant.setReady(true);
        waitOnPlayers();

        // set max score
        {
            std::unique_lock<std::mutex> l(_mutex);
            if(contestant.getScore() > _maxScore)
                _maxScore = contestant.getScore();
        }
        
        rBody += std::to_string(_maxScore);
        conn.write(Message('o', rBody).serialize());
    }
    
}

void ContestSession::waitOnPlayers(void)
{
    bool roundReady = false;
    while(!roundReady)
    {
        std::unique_lock<std::mutex> l(_mutex);
        for(auto p : _players)
            if(!p.second.isReady()) 
                roundReady = false;
    }
    // TODO ^ : use conditional variables instead of a spin-lock
}*/