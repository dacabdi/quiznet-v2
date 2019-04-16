#ifndef __UTILS__H__
#define __UTILS__H__

#include "Common.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace utils
{
    std::vector<std::string> split(
        const std::string& str,
        const std::string& delimiter = ",");

    char * escape(const char *);
    std::string escape(const std::string& ref);

    std::string trim(const std::string& str,
                     const std::string& whitespace = " \n\t\r");

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

    // compare double and float
    template<class T>
    typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
        almost_equal(T x, T y, int ulp = 2)
    {
        return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
            || std::abs(x-y) <  std::numeric_limits<T>::min();
    }
}

#endif