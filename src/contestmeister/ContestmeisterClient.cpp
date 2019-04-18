#include "ContestmeisterClient.h"

ContestmeisterClient::ContestmeisterClient(Host host, 
                                           std::istream& is, 
                                           std::ostream& os,
                                           std::ostream& es)
: _host(host), _is(is), _os(os), _es(es)
{
    init_handlers();
}

void ContestmeisterClient::run(void)
{
    if(connect())
        prompt_loop();
}

void ContestmeisterClient::prompt_loop(void)
{
    _os << _menu;

    char option = '\0';
    std::string prompt = "> ";
    std::string invalidMsg = "Invalid option, please try again.";

    _running = true;
    while(_running)
    {
        _os << prompt << std::flush;
        
        std::string line = "";
        std::getline(_is, line);
        
        size_t pos = line.find_first_not_of(" \t\n\r");
        if(pos == std::string::npos)
            continue;

        line = line.substr(pos);
        option = line.at(0);

        if (option == 'h') { // options that do not require server comm
            _os << _menu; continue;
        } else if(!_handlers.count(option)) { // unkown request
            _es << invalidMsg << std::endl;
        } else {
            (_handlers.at(option))(line);
        }
    }
}

bool ContestmeisterClient::connect(void)
{
    Message res;
    try {
        _sock.Connect(_host);
        _sock.write(Message('n',"contestmeister\n").serialize());
        std::string res_str = _sock.read();
        res = Message(res_str);
    } catch (const Except& e) {
        _es << "Failed to connect to server: " 
            << e.what() << std::endl;
        return false;
    }

    return res.type() == 'o';
}

void ContestmeisterClient::init_handlers(void)
{
    _handlers.emplace('p', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        uint32_t id = 0;
        try { id = std::stoul(line.substr(1)); }
        catch (const std::exception& e){ _os << "Invalid request format" << std::endl; return; }

        SolvedQuestion sq(_is);
        Message req('p', std::to_string(id) + "\n" + sq.serialize());
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();
        if(res_str.size() == 0){
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }
        
        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content() << std::endl;
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('d', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        uint32_t id = std::stoul(line.substr(1));
        Message req('d', std::to_string(id) + "\n");
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0){
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content() << std::endl;
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('g', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        uint32_t id = std::stoul(line.substr(1));
        Message req('g', std::to_string(id) + "\n");
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0){
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content();
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('q', [&](std::string line)
    {
        Message req('q', "");
        _sock.write(req.serialize());
        _running = false;

        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0){
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << "Server knows client is leaving." << std::endl;
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('k', [&](std::string line)
    {
        Message req('k', "");
        _sock.write(req.serialize());   

        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0) {
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << "Server agreed to die." << std::endl;
            _running = false;
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('s', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        uint32_t id = std::stoul(line.substr(1));
        Message req('s', std::to_string(id) + "\n");
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0) {
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content() << std::endl;
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('a', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        Message req('a', (line.substr(2)) + "\n");
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0) {
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content() << std::endl;
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('b', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        Message req('b', (line.substr(2)) + "\n");
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0) {
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content() << std::endl;
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('l', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        Message req('l', "");
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0) {
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content();
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });

    _handlers.emplace('r', [&](std::string line)
    {
        // send message TODO handle exceptions in this process
        Message req('r', (line.substr(2)) + "\n");
        _sock.write(req.serialize());
        
        // wait for response
        std::string res_str = _sock.read();

        if(res_str.size() == 0) {
            _os << "Server seems to have dropped the connection" << std::endl;
            return;
        }

        Message res(res_str);
    
        // treat OK response
        if(res.type() == 'o') {
            _os << res.body().content();
            return;
        }

        // treat error response
        if(res.type() == 'e')
        {
            ProtoError e(res.body().content());
            _os << e.extra() << std::endl;
            return;
        }

        _os << "Unkown server response" << std::endl;
    });
}
