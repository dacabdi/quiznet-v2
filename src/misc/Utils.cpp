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

char * utils::escape(const char * cstr)
{
    std::string sstr = utils::escape(std::string(cstr));
    char * buff = new char[sstr.size()+1];
    sstr.copy(buff, sstr.size());
    return buff;
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

std::string utils::trim(const std::string& str,
                        const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) return "";

    ssize_t strEnd = str.find_last_not_of(whitespace);
    ssize_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}