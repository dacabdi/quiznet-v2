#ifndef __ESCAPE__H__
#define __ESCAPE__H__

#include <ostream>

// Only used for display purposes

// TODO move to utils

struct EscapedString
{
    const char* str;

    friend inline std::ostream& operator<<(std::ostream& os, const EscapedString& e)
    {
        for (const char* char_p = e.str; *char_p != '\0'; char_p++)
        {
            switch (*char_p)
            {
                case '\a':  os << "\\a"; break;
                case '\b':  os << "\\b"; break;
                case '\f':  os << "\\f"; break;
                case '\n':  os << "\\n"; break;
                case '\r':  os << "\\r"; break;
                case '\t':  os << "\\t"; break;
                case '\v':  os << "\\v"; break;
                case '\\':  os << "\\\\"; break;
                case '\'':  os << "\\'"; break;
                case '\"':  os << "\\\""; break;
                case '\?':  os << "\\\?"; break;
                default: os << *char_p;
            }
        }
        return os;
    }
};

#endif