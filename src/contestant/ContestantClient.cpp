#include "ContestantClient.h"


ContestantClient::ContestantClient(Host host, std::istream& is, 
                                              std::ostream& os,
                                              std::ostream& es)
: _host(host), _is(is), _os(os), _es(es)
{}

void ContestantClient::run(void)
{
    if(joinContest())
        playAllRounds();
}

bool ContestantClient::joinContest(void)
{
    TcpSocket conn;
    std::string name_buffer;
    
    // attempt connection
    try {
        _os << "Connecting to a contest on " << _host.fullName() << " ... " << std::flush;
        conn.Connect(_host); _os << "OK" << std::endl;
    } catch (const Except& e) {
        _os << "[FAILED]\n" << std::endl;    
        _es << e.what() << std::endl;
        return false;
    }

    // input nickname
    bool in_use = true;
    do
    {
        _os << "Please input a nickname : ";
        _is >> name_buffer;
        
        // send handshake with name
        Message handshake('n', "contestant\n" + name_buffer + "\n");
        conn.write(handshake.serialize());

        // read response
        std::string handshakeResString = conn.read();
        Message handshakeRes(handshakeResString);
        switch(handshakeRes.type())
        {
            case 'e' : 
            {
                ProtoError err(handshakeRes.body().content());
                switch (err.number())
                {
                    case NCKDUP:
                        _es << "Error: Nickname " << err.extra() << " is already in use." << std::endl; 
                    break;

                    case EMPTYN: // probably empty contestant name
                        _es << "Error:  " << err.msg() << "." << std::endl;
                    break;
                
                    default:
                        _es << "Error: An unhandled error has ocurred. Contestant client will stop." << std::endl;
                        _es << err.present() << std::endl;

                        return false;
                    break;
                }
            }

            break;

            case 'o' : // OK

                in_use = false;

            break;

            default:
                _es << "Error: Unrecognized server response type during handshake." << std::endl;
                _es << handshakeRes.serialize() << std::endl;
                return false;
            break;
        }
    } while (in_use);
    
    _contestant = Contestant(name_buffer, std::move(conn));

    return true;
}

void ContestantClient::playAllRounds(void)
{
    _os << "Hello " << _contestant.Nickname 
        << ", get ready for contest!" << std::endl;

    bool terminated = false;
    uint32_t round = 1;
    Message round_msg;

    while(!terminated)
    {
        round_msg = Message(_contestant.Conn.read());

        switch(round_msg.type())
        {
            case 't':

                _os << "The contest is over - thanks for playing "
                    << _contestant.Nickname << "!" << std::endl;
                
                // ack termination
                _contestant.Conn.write(Message('o', "").serialize());

                terminated = true;

            break;

            case 'u':
                
                playRound(round_msg, round);
            
            break;

            default:

                _es << "Error: Received unkown type of message from server: " << std::endl;
                _es << round_msg.serialize();
                return;

            break;
        }
    }
}

void ContestantClient::playRound(Message round_msg, uint32_t round)
{
    Question q(round_msg.body().content());
    char choice = '\0';

    // present question and read answer
    _os << "Question " << round++ << ":" << std::endl; 
    _os << q.present();

    // validate choice (make sure it exists in the question)
    bool choice_valid;
    do
    {
        _os << "Enter your choice: ";
        _is >> choice;

        if(q.getChoices().hasChoice(choice))
            choice_valid = true;
        else
        {
            _es << "Provided " << choice
                << " is not a valid choice for this question."
                << " Try again." << std::endl;
        }
    } while (!choice_valid);
    
    // send answer
    Message q_ans_msg = Message('u', std::string(1, choice));
    _contestant.Conn.write(q_ans_msg.serialize());

    // read results
    std::string res_msg_str = _contestant.Conn.read();
    Message res_msg(res_msg_str);

    // build results
    RoundResults rr = RoundResults::deserialize(res_msg.body().content());
    _os << rr.present();
    _os << std::endl;

    // ack results
    Message res_ack_msg('o',"");
    _contestant.Conn.write(res_ack_msg.serialize());
}