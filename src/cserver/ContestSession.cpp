#include "ContestSession.h"

ContestSession::ContestSession(TcpSocket& sock, std::map<uint32_t, SolvedQuestion>& questions)
: _sock(sock), _questionsRef(questions), _rounds(0)
{
    // socket must be already bound
    if(!sock.bound())
        throw Except("Socket not bound", "ContestSession::start()", "", false);

    std::thread timeout_signal([&](){
        std::this_thread::sleep_for(std::chrono::seconds(60));
        _sock.Close();
    });

    bool timeout = false;
    while(!timeout)
    {
        try {
            playerThread(_sock.Accept());
        } catch(const Except& e) {
            if(e.Errno == EINVAL) timeout = true;
            else throw e;
        }
    }

    timeout_signal.join();
}

void ContestSession::playerThread(TcpSocket conn)
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
        std::istringstream iss = msg.body.content();

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
        char playerChoice = msg.body.content().at[0];

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
        
        
    }
    
        waitOnPlayers();
        // wait for all players...
    
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
}