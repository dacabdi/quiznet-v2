#include "Except.h"

Except::Except(const std::string& what, const std::string& where, 
               const std::string& extra, bool includeErrstr)
: Except(what, where, extra, includeErrstr, errno)
{}

Except::Except(void) 
: Except("", "", "", false, 0)
{}

const char * Except::what () const throw ()
{
    return _what.c_str();
}

// init protected constructor
Except::Except(
    const std::string& what, const std::string& where, 
    const std::string& extra, bool includeErrstr,
    const size_t errnumber)
: std::runtime_error("Quiznet Runtime Exception"),
  What(what), 
  Where(where), 
  Extra(extra), 
  IncludeErrstr(includeErrstr),
  Errno(errnumber)
{
    _what = compose();
}

std::string Except::compose(void) const
{
    std::string s(
          "\nWhat: "  + What  + "\n" 
        +   "Where: " + Where + "\n"
        +   "Extra: " + Extra + "\n"
    );

    if(IncludeErrstr)
    {
        char buff[1024]; bzero(buff, 1024);
        int r = strerror_r((int)Errno, buff, 1024);
        s += "Errno: " + std::to_string(Errno) + "\n"
            +  "Errstr: " + (!r ? buff : "Could not retrieve error string") + "\n";
    } 

    return s;
}