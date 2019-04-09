#include "ProtoExcept.h"

ProtoExcept::ProtoExcept(ssize_t id, const std::string& extra)
: ProtoExcept(ProtoError(id, extra))
{}

ProtoExcept::ProtoExcept(const ProtoError& e)
: Error(e)
{
    _what = Error.present();
}

const char * ProtoExcept::what () const throw ()
{
    return _what.c_str();
}