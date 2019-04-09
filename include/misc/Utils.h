#ifndef __UTILS__H__
#define __UTILS__H__

#include "Error.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace utils
{
    std::vector<std::string> split(
        const std::string& str,
        const std::string& delimiter = ",");

    std::string escape(const std::string& ref);

    std::string trim(const std::string& str,
                     const std::string& whitespace = " \n\t\r");

    struct ErrorMessage deserializeError(std::string str);

    // templated functions cannot be defined in object files
    template<typename K, typename V>
    bool mapsEqual(const std::map<K, V>& m1, const std::map<K, V>& m2)
    {
        typename std::map<K, V>::const_iterator it1 = m1.begin();
        typename std::map<K, V>::const_iterator it2 = m2.begin();

        for(; it1 != m1.end() && it2 != m2.end(); ++it1, ++it2)
            if(    (*it1).first  != (*it2).first 
                || (*it1).second != (*it2).second)
                        return false;

        // one of them hasn't gotten to the end
        if(it1 != m1.end() || it2 != m2.end())
            return false;

        return true;
    }
}

#endif