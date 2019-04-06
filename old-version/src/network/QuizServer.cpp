#include "QuizServer.h"

QuizServer::QuizServer(IQuizBook* quizbook, 
                       IHost* host, 
                       ISocket* socket,
                       const std::string& filename)
{
    _host = host;
    _quizbook = quizbook;
    _socket = socket;
    _filename = filename;

    // before setting handlers,
    // reset quizbook and load with file
    _quizbook->clear();
    std::ifstream ifs(_filename);
    _quizbook->readFrom(ifs);

    // set quizbook event handlers
    _quizbook->onInsert = [&](const ISolvedQuestion& question, 
                              IQuizBook* book) { onChange(book); };
    _quizbook->onDelete = [&](ISolvedQuestion& question, 
                              IQuizBook* book) { onChange(book); };
    _quizbook->onClear  = [&](IQuizBook* book) { onChange(book); };

    // set each request type handler
    setRequestHandlers();

    // set incoming connection on socket handler
    _socket->onIncomingConnection = [&]
    (ISocket& socket, const IHost& host, ISocket* context) 
    {
        _log("\n-=-=-=-=-=-=-=- Incoming connection -=-=-=-=-=-=-=-\n\n");

        std::cout << "[ ON   : " << std::right << std::setw(16) 
                                 << socket.getAddress() << ":"
                                 << std::left << std::setw(5)
                                 << socket.getPort()
                                 << " ]";

        std::cout << " <- " << std::flush;

        std::cout << "[ FROM : " << std::right << std::setw(16) 
                                 << host.getAddress() << ":"
                                 << std::left << std::setw(5)
                                 << host.getPort()
                                 << " ]\n";
        
        std::cout << std::flush;

        _clientOn = true;
        
// ======== NEGOTIATE SESSION TYPE =======================================
        
        bool _sessionNegotiated = false;
        
        // determine client type
        bool _persistent = false;
        try
        {
            _log("\nNegotiating session...\n");
            Request greetings(socket.readFromSocket());
            if(greetings.getType() == 's') 
            {
                _log("Client wants " 
                + greetings.getBody().getContent() + ".\n");

                std::string responseContent = "";
                if(greetings.getBody().getContent() == "persistent")
                {
                    responseContent = "persistent";
                    _persistent = true;
                }
                else if (greetings.getBody().getContent() == "nonpersistent")
                {
                    responseContent = "nonpersistent";
                    _persistent = false;
                } // TODO:  validate that client
                  // actually asked for nonpersistent
                  // this takes it implicitly
                Response response('o', responseContent);
                socket.writeToSocket(response.serialize());
                _sessionNegotiated = true;
                _log("Session negotiated succesfully!\n\n");
            } else {
                _log("Could not negotiate session, wrong type of request!\n");
                if (_verbose)
                    _log("Received this: "
                    + utils::escape(greetings.serialize()));
            }
        } catch (const std::exception& e) {
            _log("Client left without notice during session negotiation!\n");
            if(_verbose) _log(std::string(e.what()) + "\n");
        }

// ======== RUN SESSION ==================================================

        // communicate
        if(_sessionNegotiated)
            try {    
                if(_persistent) persistentSession(socket);
                else            exchange(socket);
            } catch(const std::exception& e) {
                _log("\nClient left without notice!\n");
                if(_verbose) _log(std::string(e.what()) + "\n");
            }

        _clientOn = false;
        
        _log("\n-=-=-=-=-=-=-=-=- Connection ended -=-=-=-=-=-=-=-=-\n\n");
    };    
}

void QuizServer::exchange(ISocket& socket)
{
    _log("Awaiting request...\n");
    Response response = attendRequest(socket);
    _log("Sending response... \n");
    socket.writeToSocket(response.serialize());
    _log("Done sending response\n\n");
}

void QuizServer::persistentSession(ISocket& socket)
{
    while(_running && _clientOn)
    {
        exchange(socket);
    }
}

void QuizServer::setLogger(std::function<void (const std::string&)> logger)
{
    _log = logger;
}

void QuizServer::run(void)
{
    std::cout << "<><><><><><><><><>"
              << " Starting QuizNet Server"
              << " <><><><><><><><><>\n\n"
              << std::flush;
        
    // bind
    _socket->bindSocket(*(_host));
    // listen
    _socket->startListening();

    std::cout << "Listening on " 
              << _socket->getAddress()
              << ":"
              << _socket->getPort()
              << std::endl << std::endl;

    _running = true;
    while(_running)
    {
        _log("Accepting connection...\n");
        _socket->acceptConnection();
    }
}

void QuizServer::onChange(IQuizBook* book)
{
    // open the file ...
    std::ofstream file(_filename, std::ios::out);
    book->writeTo(file); //... and write quizbook to it
    file.close();
}

Response QuizServer::attendRequest(ISocket& socket)
{       
    Request request;
    // try to parse request
    try {
        request = readRequest(socket);
    }
    catch(const ProtocolException& e) {
        _log("Failed to parse request!\n");
        if(_verbose){
            _log(  "-=-=- Exception begin -=-=-\n");
            _log(e.what());
            _log("\n-=-=- Exception end -=-=-\n");
        }
        return Response('e', std::string(e.what()));
    }

    // try
    _log("Composing response...\n");
    Response response;
    try {
        response = (_handlers.at(request.getType()))(request);
    } catch (const ProtocolException& e) {
        _log("Error composing response!\n");
        if(_verbose){
            _log(  "\n-=-=- Exception begin -=-=-\n");
            _log(e.what());
            _log("\n-=-=-= Exception end =-=-=-\n\n");
        }
        _log("Composing error response...\n");
        return Response('e', std::string(e.what()));
    }

    if(_verbose) _log("Response: " + utils::escape(response.serialize()) + "\n");
    _log("Done composing response\n");

    return response;
}

Request QuizServer::readRequest(ISocket& socket)
{
    _log("Reading request...\n");
    std::string buffer = socket.readFromSocket();
    _log("Request read.\n");

    // detect if reading 0, if so, consider client dropped
    // https://stackoverflow.com/questions/283375/detecting-tcp-client-disconnect
    if(!buffer.length())
        throw Exception("Client dropped without notice", 
                        "QuizServer::readRequest", "", false);

    _log("Parsing request...\n");
    Request request(buffer);
    _log("Request parsed.\n");

    // log content and type
    _log("Type: " + request.getTypeAsString() + "\n");
    _log("Content length: " 
        + std::to_string(request.getBody().getLength()) + "\n");
    if(_verbose)
        _log("Content: "
        + utils::escape(request.getBody().getContent()) + "\n");

    return request;
}

void QuizServer::setRequestHandlers(void)
{
    // p -> put a question in the bank
    _handlers.emplace('p', [&](const Request& request) -> Response
    {
        std::string content = request.getBody().getContent();
        try {
            // create question
            SolvedQuestion question(content);
            uint32_t id = _quizbook->insertQuestion(question);
            return Response('o', std::to_string(id));
        } catch (const Exception& e) {
            throw ProtocolException(MALQUE, e.what());
        } catch (const std::exception& e) {
            throw ProtocolException(UNKERR, 
            "Content: {" + utils::escape(content) + "}\n"
            "Internal Exception: " + std::string(e.what()));
        }
    });

    //d -> delete a question from the bank
    _handlers.emplace('d', [&](const Request& request) -> Response
    {    
        std::string content = request.getBody().getContent();
        try {
            uint32_t id = (uint32_t)std::stoul(content);
            _quizbook->deleteQuestionById(id);
            return Response('o', "Deleted question " + std::to_string(id));
        } catch (const Exception& e) {
            throw ProtocolException(NOTFND, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what()));
        } catch (const std::exception& e) {
            throw ProtocolException(UNKERR, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what()));
        }
    });

    //g -> get a question from the bank
    _handlers.emplace('g', [&](const Request& request) -> Response
    {   
        std::string content = request.getBody().getContent();
        try {
            uint32_t id = (uint32_t)std::stoul(content);
            SolvedQuestion q = _quizbook->getQuestionById(id);
            return Response('o', q.serialize());
        } catch (const Exception& e) {
            throw ProtocolException(NOTFND, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what()));
        } catch (const std::exception& e) {
            throw ProtocolException(UNKERR, 
            "Question id provided {" + utils::escape(content) + "}\n"+
            "Internal Exception: " + std::string(e.what()));
        }
    });

    //c -> check a solution
    _handlers.emplace('c', [&](const Request& request) -> Response
    {   
        std::string content = request.getBody().getContent();
        
        std::istringstream iss(content);
        std::string idString = "";
        std::getline(iss, idString);
        std::string choiceString = "";
        std::getline(iss, choiceString);

        char choice = choiceString.front();

        SolvedQuestion question;

        try {
            uint32_t id = (uint32_t)std::stoul(content);
            question = _quizbook->getQuestionById(id);
        } catch (const Exception& e) {
            throw ProtocolException(NOTFND, 
            "Question id provided {" + utils::escape(idString) + "}\n"+
            "Internal Exception: " + std::string(e.what()));
        } catch (const std::exception& e) {
            throw ProtocolException(UNKERR, 
            "Question id provided {" + utils::escape(idString) + "}\n"+
            "Internal Exception: " + std::string(e.what()));
        }

        if(!question.getQuestion().getChoices().hasChoice(choice))
        {
            throw ProtocolException(CHNFND, 
            "Choice id provided {" + utils::escape(choiceString) + "}\n");
        }

        if(question.getSolution() == choice)
            return Response('o', "Correct");
        else    
            return Response('o', "Incorrect");
    });

    //r -> get a random question from the bank
    _handlers.emplace('r', [&](const Request& request) -> Response
    {
        try {
            uint32_t id;
            Question question = _quizbook->getRandomQuestion(id).getQuestion();
            return Response('o', std::to_string(id) + "\n" + question.serialize());
        } catch (const Exception& e) {
            throw ProtocolException(EMPTYQ, "The QuizBook is empty!");
        } catch (const std::exception& e) {
            throw ProtocolException(UNKERR, 
            "Attempting to retrieve random question");
        }
    });

    // q -> quit client
    _handlers.emplace('q', [&](const Request& request) -> Response
    {
        _log("Client requested to disconnect.\n");
        _clientOn = false;
        return Response('o', "OK");
    });

    // k -> kill server from client
    _handlers.emplace('k', [&](const Request& request) -> Response
    {
        _log("Server received a kill request.\n");
        _running = false;
        return Response('o', "OK");
    });
}