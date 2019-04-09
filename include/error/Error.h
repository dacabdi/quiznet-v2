#ifndef __ERROR__H__
#define __ERROR__H__

#include <map>
#include <string>

#define UNKERR  0
#define NOTFND  1
#define MALQUE  2
#define CHNFND  3 
#define UNKREQ  4 
#define INVQID  5
#define EMPTYQ  6
#define REQTSZ  7
#define USEDID  8
#define CTNFND  9
#define LATEJN 10
#define FAILGT 11

struct ErrorMessage {
    size_t number;
    std::string symbol;
    std::string message;
    std::string extra;
};

const static std::map<size_t, ErrorMessage> Errors = {
    {UNKERR, {UNKERR, "UNKERR", "Unkown error", ""}},
    {NOTFND, {NOTFND, "NOTFND", "Question not found", ""}},
    {MALQUE, {MALQUE, "MALQUE", "Malformed question body", ""}},
    {CHNFND, {CHNFND, "CHNFND", "Choice does not exist in question", ""}},
    {UNKREQ, {UNKREQ, "UNKREQ", "Server does not know how to handle request type", ""}},
    {INVQID, {INVQID, "INVQID", "Invalid format for question id provided", ""}},
    {EMPTYQ, {EMPTYQ, "EMPTYQ", "Empty quiz book", ""}},
    {REQTSZ, {REQTSZ, "REQTSZ", "Request type length must be one character", ""}},
    {USEDID, {USEDID, "USEDID", "The proposed id is already in use", ""}},
    {CTNFND, {CTNFND, "CTNFND", "Contest not found", ""}},
    {LATEJN, {LATEJN, "LATEJN", "Contest has already started", ""}},
    {FAILGT, {FAILGT, "FAILGT", "Failed to negotiate session.", ""}}
};

#endif