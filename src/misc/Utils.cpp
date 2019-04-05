#include "Utils.h"

std::vector<std::string> utils::split(
    const std::string& str, 
    const std::string& delimiter)
{
    std::string s = str;
    std::vector<std::string> output;
    size_t pos = 0;
    std::string token;

    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        output.push_back(token);
        s.erase(0, pos + delimiter.length());
    }

    output.push_back(s);

    return output;
}


std::string utils::escape(const std::string& ref)
{
    std::ostringstream oss;
    for (const char* char_p = ref.c_str(); *char_p != '\0'; char_p++)
    {
        switch (*char_p)
        {
            case '\a':  oss << "\\a"; break;
            case '\b':  oss << "\\b"; break;
            case '\f':  oss << "\\f"; break;
            case '\n':  oss << "\\n"; break;
            case '\r':  oss << "\\r"; break;
            case '\t':  oss << "\\t"; break;
            case '\v':  oss << "\\v"; break;
            case '\\':  oss << "\\\\"; break;
            case '\'':  oss << "\\'"; break;
            case '\"':  oss << "\\\""; break;
            case '\?':  oss << "\\\?"; break;
            default: oss << *char_p;
        }
    }
        
    return oss.str();
}

struct ErrorMessage utils::deserializeError(std::string str)
{
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

    return { number, symbol, message, extra };
}


std::string utils::trim(const std::string& str,
                        const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) return "";

    ssize_t strEnd = str.find_last_not_of(whitespace);
    ssize_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}