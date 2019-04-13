#include "ContestSession.h"

ContestSession::ContestSession(TcpSocket& sock, std::map<uint32_t, SolvedQuestion>& questions)
: _sock(sock), _questionsRef(questions), _rounds(0)
{
    // socket must be already bound
    if(!sock.bound())
        throw Except("Socket not bound", "ContestSession::start()", "", false);

    // get players
}

void ContestSession::readPlayers(void)
{
    std::thread window_signal([&](){
        std::this_thread::sleep_for(std::chrono::seconds(60));
        _sock.Close();
    });

    //for()
}



void start(TcpSocket& sock)
{
    // check if socket is bound
}