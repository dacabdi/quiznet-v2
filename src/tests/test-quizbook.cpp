#include "Exception.h"
#include "QuizBook.h"
#include "Utils.h"
#include "Escape.h"

#include <iostream>
#include <fstream>
#include <unistd.h>

bool quizBookLoadOneQuestion(void);
bool quizBookLoadNQuestion(void);
bool quizBookInsertDelete(void);
bool quizBookSaveAndReloadUsingEvents(void);

void runTest(const std::string& name, bool test(void));

int main(void)
{
    char cCurrentPath[FILENAME_MAX] = "";

    if (!getcwd(cCurrentPath, sizeof(cCurrentPath)))
    {
        return errno;
    }

    std::cout << "Reading test data files from: " 
              << cCurrentPath << std::endl;

    runTest("quizBookLoadOneQuestion", quizBookLoadOneQuestion);
    runTest("quizBookLoadNQuestion", quizBookLoadNQuestion);
    runTest("quizBookInsertDelete", quizBookInsertDelete);
    runTest("quizBookSaveAndReloadUsingEvents", 
        quizBookSaveAndReloadUsingEvents);

    return 0;
}

void runTest(const std::string& name, bool test(void))
{
    std::cout << name << ":" << std::flush;
    
    if(!test())
        std::cout << "[FAILED]" << std::flush;
    else
        std::cout << "[OK]" << std::flush;
    
    std::cout << std::endl;
}

bool quizBookLoadOneQuestion(void)
{
    std::ifstream fs("test-quizbook-load-one.data");
    QuizBook qb;
    qb.readFrom(fs);
    fs.close();

    std::string expectedSerialization(
        "23\n"
        "presidents, US history\n"
        "Which is the first president of the USA\n"
        ".\n"
        "(a) Thomas Jefferson\n"
        ".\n"
        "(b) Abraham Lincoln\n"
        ".\n"
        "(c) George Washington\n"
        ".\n"
        "(d) Benjamin Franklin\n"
        ".\n"
        ".\n"
        "c\n"
    );

    if(qb.serialize() != expectedSerialization)
        return false;

    return true;
}

bool quizBookLoadNQuestion(void)
{
    std::ifstream fs("test-quizbook-load-n.data");
    QuizBook qb;
    qb.readFrom(fs);
    fs.close();

    std::string expectedSerialization(
    "23\npresidents, US history\nWhich is the first president of the "
    "USA\n.\n(a) Thomas Jefferson\n.\n(b) Abraham Lincoln\n.\n(c) "
    "George Washington\n.\n(d) Benjamin Franklin\n.\n.\nc\n\n43\nnetwork"
    ", protocols,layered\nWhich of these protocols guarantee delivery"
    " \n.\n(a) TCP\n.\n(b) UDP\n.\n.\na\n\n54\napplication, protocols, "
    "DNS\nWhich application protocol provides\nhostname to IP address"
    " resolution\n.\n(a) FTP\n.\n(b) HTTP\n.\n(c) DNS\n.\n(d) SMTP\n."
    "\n.\nd\n\n57\nnetwork, protocols,layered\nWhich of these does not b"
    "elong in the Internet protocol stack model\n.\n(a) presentation"
    "\n.\n(b) transport\n.\n(c) link\n.\n(d) network\n.\n.\na\n\n89\nar"
    "chitecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
    "(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
    
    SolvedQuestion q1(
    "presidents, US history\nWhich is the first president of the "
    "USA\n.\n(a) Thomas Jefferson\n.\n(b) Abraham Lincoln\n.\n(c) "
    "George Washington\n.\n(d) Benjamin Franklin\n.\n.\nc\n");
    uint32_t q1_Id = 23; 

    SolvedQuestion q2(
    "network"
    ", protocols,layered\nWhich of these protocols guarantee delivery"
    " \n.\n(a) TCP\n.\n(b) UDP\n.\n.\na\n");
    uint32_t q2_Id = 43;

    SolvedQuestion q3(
    "application, protocols, "
    "DNS\nWhich application protocol provides\nhostname to IP address"
    " resolution\n.\n(a) FTP\n.\n(b) HTTP\n.\n(c) DNS\n.\n(d) SMTP\n."
    "\n.\nd\n");
    uint32_t q3_Id = 54;

    SolvedQuestion q4(
    "network, protocols,layered\nWhich of these does not b"
    "elong in the Internet protocol stack model\n.\n(a) presentation"
    "\n.\n(b) transport\n.\n(c) link\n.\n(d) network\n.\n.\na\n");
    uint32_t q4_Id = 57;

    SolvedQuestion q5(
    "architecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
    "(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
    uint32_t q5_Id = 89;

    if(qb.serialize() != expectedSerialization)
        return false;

    if(qb.size() != 5)
        return false;

    if(qb.getQuestionById(q1_Id) != q1)
        return false;
   
    if(qb.getQuestionById(q2_Id) != q2)
        return false;

    if(qb.getQuestionById(q3_Id) != q3)
        return false;

    if(qb.getQuestionById(q4_Id) != q4)
        return false;

    if(qb.getQuestionById(q5_Id) != q5)
        return false;

    // it has to be equal to at least one
    SolvedQuestion rQ = qb.getRandomQuestion();
    if(rQ != q1
    && rQ != q2
    && rQ != q3
    && rQ != q4
    && rQ != q5)
        return false;

    if(!qb.hasQuestion(q1_Id)
    || !qb.hasQuestion(q2_Id)
    || !qb.hasQuestion(q3_Id)
    || !qb.hasQuestion(q4_Id)
    || !qb.hasQuestion(q5_Id))
        return false;

    if(qb.hasQuestion(100)
    || qb.hasQuestion(200)
    || qb.hasQuestion(300))
        return false;
    
    if(qb.deleteQuestionById(q1_Id) != q1)
        return false;

    if(qb.size() != 4)
        return false;

    if(qb.deleteQuestionById(q2_Id) != q2)
        return false;

    qb.clear();

    if(qb.size() != 0)
        return false;

    uint32_t newId = qb.insertQuestion(q1);
    
    if(qb.size() != 1)
        return false;

    if(qb.deleteQuestionById(newId) != q1)
        return false;

    return true;
}

bool quizBookInsertDelete(void)
{
    SolvedQuestion q(
    "architecture, design\nBitTorrent is an example of\n.\n(a) P2P\n.\n"
    "(b) client/server\n.\n(c) hierarchy\n.\n(d) hybrid\n.\n.\nd\n");
    uint32_t qId = 89;

    SolvedQuestion copy = q;

    QuizBook qb;

    if(qb.insertQuestion(qId, q) != qId)
        return false;
    
    q = qb.deleteQuestionById(qId);
    qb.clear();

    if(q != copy)
        return false;

    return true;
}

bool quizBookSaveAndReloadUsingEvents(void)
{
    QuizBook qb1;

    qb1.onInsert = [] (const ISolvedQuestion& q, IQuizBook* qb) {
        std::ofstream file("test-quizbook-output1.data");
        qb->writeTo(file);
        file.close();
    };
    
    SolvedQuestion q1(
    "presidents, US history\nWhich is the first president of the "
    "USA\n.\n(a) Thomas Jefferson\n.\n(b) Abraham Lincoln\n.\n(c) "
    "George Washington\n.\n(d) Benjamin Franklin\n.\n.\nc\n");
    uint32_t q1_Id = 23; 

    SolvedQuestion q2(
    "network"
    ", protocols,layered\nWhich of these protocols guarantee delivery"
    " \n.\n(a) TCP\n.\n(b) UDP\n.\n.\na\n");
    uint32_t q2_Id = 43;

    SolvedQuestion q3(
    "application, protocols, "
    "DNS\nWhich application protocol provides\nhostname to IP address"
    " resolution\n.\n(a) FTP\n.\n(b) HTTP\n.\n(c) DNS\n.\n(d) SMTP\n."
    "\n.\nd\n");
    uint32_t q3_Id = 54;

    qb1.insertQuestion(q1_Id, q1);
    qb1.insertQuestion(q2_Id, q2);
    qb1.insertQuestion(q3_Id, q3);

    qb1.clear();
    
    std::ifstream file("test-quizbook-output1.data");
    qb1.readFrom(file);
    
    //qb1.serialize();

    return true;
}