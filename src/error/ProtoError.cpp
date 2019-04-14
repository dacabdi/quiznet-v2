#include "ProtoError.h"

ProtoError::ProtoError(size_t number, const std::string& symbol, const std::string& msg, const std::string& extra)
: _number(number), _symbol(symbol), _msg(msg), _extra(extra)
{}

ProtoError::ProtoError(size_t number, const std::string& extra)
: ProtoError(number, ProtoError::Types.at(number).symbol(), ProtoError::Types.at(number).msg(), extra)
{}

ProtoError::ProtoError(const std::string& str)
: ProtoError(ProtoError::deserialize(str))
{}

ProtoError::ProtoError(void)
: ProtoError(0, "", "", "")
{}

size_t ProtoError::number(void) const
{
    return _number;
}

std::string ProtoError::symbol(void) const
{
    return _symbol;
}

std::string ProtoError::msg(void) const
{
    return _msg;
}

std::string ProtoError::extra(void) const
{
    return _extra;
}

ProtoError ProtoError::deserialize(const std::string& s)
{
    try {
        std::string str = s;
        size_t pos = str.find_first_of("\n");

        size_t number = std::stoul(str.substr(0, pos));
        str.erase(0, pos+1);

        pos = str.find_first_of("\n");
        std::string symbol = str.substr(0, pos);
        str.erase(0, pos+1);

        pos = str.find_first_of("\n.\n");
        std::string message = str.substr(0, pos);
        str.erase(0, pos+3);

        pos = str.find_first_of("\n.\n");
        std::string extra = str.substr(0, pos);
        str.erase(0, pos+3);

        return ProtoError(number, symbol, message, extra);
    } catch(const std::exception& e) {
        throw Except("Failed to deserialize protocol error", 
                     "ProtoError::deserialize()",
                     "InnerExceptionString : " + std::string(e.what()) + "\n" +
                     "PassedString : " + s + "\n", false);
    }
}

std::string ProtoError::serialize(void) const
{
    std::ostringstream oss;

    oss << _number << std::endl;
    oss << _symbol << std::endl,
    oss << _msg << std::endl << "." << std::endl;
    oss << _extra << std::endl << "." << std::endl;

    return oss.str();
}

std::string ProtoError::present(void) const
{
    std::ostringstream oss;
    oss << "Protocol Exception" <<  std::endl;
    oss << "Number: " << std::to_string(_number) << std::endl;
    oss << "Symbol: " << _symbol << std::endl;
    oss << "Message: " << _msg << std::endl;
    oss << "Extra: " << _extra << std::endl;
    
    return oss.str();
}

bool ProtoError::operator==(const ProtoError& ref) const
{
    return _number == ref._number &&
           _symbol == ref._symbol &&
           _msg    == ref._msg    &&
           _extra  == ref._extra;
}

bool ProtoError::operator!=(const ProtoError& ref) const
{
    return !operator==(ref);
}

std::ostream& operator<<(std::ostream& os, const ProtoError& ref)
{
    os << ref.present();
    return os;
}

const std::map<size_t, ProtoError> ProtoError::Types = 
{
    {UNKERR, ProtoError(UNKERR, "UNKERR", "Unkown error", "")},
    {NOTFND, ProtoError(NOTFND, "NOTFND", "Question not found", "")},
    {MALQUE, ProtoError(MALQUE, "MALQUE", "Malformed question body", "")},
    {CHNFND, ProtoError(CHNFND, "CHNFND", "Choice does not exist in question", "")},
    {UNKREQ, ProtoError(UNKREQ, "UNKREQ", "Server does not know how to handle request type", "")},
    {INVQID, ProtoError(INVQID, "INVQID", "Invalid format for question id provided", "")},
    {EMPTYQ, ProtoError(EMPTYQ, "EMPTYQ", "Empty quiz book", "")},
    {REQTSZ, ProtoError(REQTSZ, "REQTSZ", "Request type length must be one character", "")},
    {USEDID, ProtoError(USEDID, "USEDID", "The proposed id is already in use", "")},
    {CTNFND, ProtoError(CTNFND, "CTNFND", "Contest not found", "")},
    {LATEJN, ProtoError(LATEJN, "LATEJN", "Contest has already started", "")},
    {FAILGT, ProtoError(FAILGT, "FAILGT", "Failed to negotiate session.", "")},
    {MSGDSL, ProtoError(MSGDSL, "MSGDSL", "Message deserialization error.", "")},
    {NCKDUP, ProtoError(NCKDUP, "NCKDUP", "Duplicate nickname provided", "")}
};