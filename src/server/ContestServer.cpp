#include "ContestServer.h"

ContestServer::ContestServer(QuizBook& qb, Host host)
: _qb(qb), _host(host), _sock(std::move(TcpSocket()))
{
    _qb.onInsert = [&](const SolvedQuestion&, QuizBook&) { writeQuestions(); };
    _qb.onDelete = [&](SolvedQuestion&, QuizBook&) { writeQuestions(); };
    _qb.onClear  = [&](QuizBook&) { writeQuestions(); };
    
    loadContests();
}

void ContestServer::SetLogger(logger log)
{
    _log = log;
}

void ContestServer::run(void)
{
    _sock.Bind(_host);
    _sock.Listen();
    
    _log("Server is listening on: " + _host.fullName());

    while(true)
    {
        try
        {
            // handle connection
            TcpSocket clientSock = _sock.Accept();
            _log("Incoming connection from: " + _sock.peer().fullName());
            
            std::thread accepting_thread([&](TcpSocket clientSock) 
            {
                if(!handshake(clientSock)) return; // failed to handshake
                requestLoop(clientSock);
            }, 
            std::move(clientSock));
        } 
        catch (const Except& e ) 
        {
            if     (e.Errno == EINVAL) break;;
            else if(e.Errno == ECONNRESET) continue; // absorb and ignore
            else throw e;
        }
    }

    _log("Server is going down!");
}

bool ContestServer::handshake(TcpSocket& conn)
{
    // read handshake
    std::string msg_str = conn.read();
    Message msg(msg_str);

    if(msg.type() != 'n' || msg.body().content() == "contestmeister\n")
        return false;

    // ack handshake
    Message h_ack('o', "");
    conn.write(h_ack.serialize());

    return true;
}

void ContestServer::requestLoop(TcpSocket& conn)
{
    bool leave = false;

    while(!leave && !_reqkill)
    {
        Message req_msg, res_msg;

        try { // client could drop dead or send problematic data
            std::string req_str = conn.read();
            Message req_msg;
        } catch (const Except& e) { 
            break;
        } catch (const ProtoExcept& e) {
            res_msg = Message('e', e.Error.serialize());
            try { conn.write(res_msg.serialize()); }
            catch (const Except& e) { break; } // could not send error message
            continue;
        }
        
        // check if we have a handler for it
        if(_handlers.count(req_msg.type()))
            res_msg = (_handlers.at(req_msg.type()))(req_msg, leave);
        else {
            ProtoError err(UNKREQ, "Request received:" + req_msg.serialize());
            try { conn.write(res_msg.serialize()); }
            catch (const Except& e) { break; } // could not send error message
            continue;
        }

        try { conn.write(res_msg.serialize()); }
        catch (const Except& e) { break; } // could not send error message
    }

    _log("Client is done!");
}

void ContestServer::initHandlers(void)
{
    // p -> put a question in the bank
    _handlers.emplace('p', [&](const Message& request, bool &leave) -> Message
    {
        std::istringstream iss = request.body().contentStream();
        std::string id_buff;
        std::getline(iss, id_buff);
        
        try {
            uint32_t id = (uint32_t)std::stoul(id_buff);
            if(_qb.hasQuestion(id))
                return Message('e', ProtoError(USEDID, id_buff).serialize());
            id = _qb.insertQuestion(id, SolvedQuestion(iss));
            return Message('o', "Question " + std::to_string(id) + " added");
        } catch (const Except& e) {
            return Message('e', ProtoError(MALQUE, e.what()).serialize());
        }
    });

    // d -> delete question by id
    _handlers.emplace('d', [&](const Message& request, bool &leave) -> Message
    {
        std::string content = request.body().content();
        try {
            uint32_t id = (uint32_t)std::stoul(content);
            _qb.deleteQuestionById(id);
            return Message('o', "Deleted question " + std::to_string(id));
        } catch (const Except& e) {
            return Message('e', ProtoError(NOTFND, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what())).serialize());
        } catch (const std::exception& e) {
            return Message('e', ProtoError(UNKERR, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what())).serialize());
        }
    });

    //g -> get a question from the bank
    _handlers.emplace('g', [&](const Message& request, bool &leave) -> Message
    {   
        std::string content = request.body().content();
        try {
            uint32_t id = (uint32_t)std::stoul(content);
            SolvedQuestion q = _qb.getQuestionById(id);
            return Message('o', q.serialize());
        } catch (const Except& e) {
            return Message('e', ProtoError(NOTFND, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what())).serialize());
        } catch (const std::exception& e) {
            return Message('e', ProtoError(UNKERR, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what())).serialize());
        }
    });

    // q -> quit client
    _handlers.emplace('q', [&](const Message& request, bool &leave) -> Message
    {
        _log("Client requested to disconnect.\n");
        leave = true;
        return Message('o', "");
    });

    // k -> kill server client
    _handlers.emplace('k', [&](const Message& request, bool &leave) -> Message
    {
        _log("Server received a kill request.\n");
        leave = true;
        _reqkill = true;
        return Message('o', "");
    });

    // s -> set contest
    _handlers.emplace('s', [&](const Message& request, bool &leave) -> Message
    {
        try {
            uint32_t id = (uint32_t)std::stoul(request.body().content());
            
            if(_contests.count(id))
                return Message('e', ProtoError(USEDID, "Error: Contest " + std::to_string(id) + " already exists").serialize());

            _contests.emplace(id, Contest(id));
            
            writeContests();
            
            return Message('o', "Contest " + std::to_string(id) + " is set");
        } catch (const Except& e) {
            return Message('e', ProtoError(MALQUE, e.what()).serialize());
        }
    });

    _handlers.emplace('a', [&](const Message& request, bool &leave) -> Message
    {   
        std::istringstream content = request.body().contentStream();
        
        uint32_t cid = 0,
                 qid = 0;

        try {
            content >> cid;
            content >> qid;
            
            if(!_contests.count(cid))
                return Message('e', ProtoError(NOTFND, "Error: Contest " + std::to_string(cid) + " does not exist").serialize());

            if(!_qb.hasQuestion(qid))
                return Message('e', ProtoError(NOTFND, "Error: Question " + std::to_string(qid) + " does not exist").serialize());

            SolvedQuestion q = _qb.getQuestionById(qid);
            _contests.at(cid).addQuestion(qid, q);

            return Message('o', "");
        } catch (const std::exception& e) {
            return Message('e', ProtoError(UNKERR, 
            "Question id provided {" + utils::escape(content.str()) + "}\n"+
            "Internal Exception: " + std::string(e.what())).serialize());
        }
    });

    _handlers.emplace('b', [&](const Message& request, bool &leave) -> Message
    {   
        std::istringstream content = request.body().contentStream();
        
        uint32_t cid = 0;

        try {
            content >> cid;
            
            if(!_contests.count(cid))
                return Message('e', ProtoError(NOTFND, "Error: Contest " + std::to_string(cid) + " does not exist").serialize());

            Contest contest = _contests.at(cid);
            ContestSession * contest_session = new ContestSession(contest.getQuestions());
            uint16_t port = contest_session->getSocket().local().port();
            std::string response = "Contest " + std::to_string(cid) + " opened on port " + std::to_string(port);
            
            std::thread th([&](ContestSession * cs, uint32_t id) -> void
            {
                cs->run();
                ContestStats stats = cs->getStats();
                _contests.at(id).setRun();
                _contests.at(id).setStats(stats);
                writeContests();
                delete cs;
            }, contest_session, cid);

            th.detach();

            return Message('o', response);
        } catch (const std::exception& e) {
            return Message('e', ProtoError(UNKERR, request.body().content()).serialize());
        }
    });

    _handlers.emplace('l', [&](const Message& request, bool &leave) -> Message
    {   
        std::ostringstream oss;
        for(auto& c : _contests)
            oss << c.second.presentHeadline();

        return Message('o', oss.str());
    });

    _handlers.emplace('r', [&](const Message& request, bool &leave) -> Message
    {   
        std::string content = request.body().content();
    
        uint32_t id = (uint32_t)std::stoul(content);
        if(!_contests.count(id))
            return Message('e', ProtoError(NOTFND, "Error: Contest " + std::to_string(id) + " does not exist").serialize());

        return Message('o', _contests.at(id).presentFull());
    });
}

void ContestServer::loadContests(void)
{
    std::ifstream file(___CONTESTS_BANK);
    while(!file.eof())  {
        Contest contest = Contest::deserialize(file);
        _contests.emplace(contest.getId(), contest);
    }
    file.close();
}
        
void ContestServer::writeContests(void)
{
    std::ofstream file(___CONTESTS_BANK, std::ios::out);
    for(auto& c : _contests) file << c.second.serialize();
    file.close();
}

void ContestServer::loadQuestions(void)
{
    std::ifstream file(___QUESTIONS_BANK);
    _qb.readFrom(file);
    file.close();
}
        
void ContestServer::writeQuestions(void)
{
    std::ofstream file(___QUESTIONS_BANK, std::ios::out);
    _qb.writeTo(file);
    file.close();
}