#ifndef __TESTS_H__
#define __TESTS_H__

#include "Except.h"
#include "ProtoExcept.h"
#include "Utils.h"

#include <iostream>
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <string.h> // strcmp
#include <vector>
#include <functional>
#include <iomanip>
#include <map>
#include <mutex>

#define __RED__   "\033[31m"
#define __GRN__   "\033[32m"
#define __YLW__   "\033[33m"
#define __RESET__ "\033[0m"
#define __CYN__   "\033[36m"
#define __MAG__   "\033[35m"

// TODO: use a singleton for all these, make atomic
std::ostringstream ____report;
std::mutex ____report_mutex;

// TODO: macro overloading

#define AssertEqual(a, b) __r__ = assertEqual(a, b) && __r__;
#define AssertNotEqual(a, b) __r__ = assertNotEqual(a, b) && __r__;

#define AssertEqualComp(a, b, comp) __r__ = assertEqual(a, b, comp) && __r__;
#define AssertNotEqualComp(a, b, comp) __r__ = assertNotEqual(a, b, comp) && __r__;

#define AssertEqualCompPrint(a, b, comp, print) __r__ = assertEqual(a, b, comp, print) && __r__;
#define AssertNotEqualCompPrint(a, b, comp, print) __r__ = assertNotEqual(a, b, comp, print) && __r__;

#define assertExcept(operation, __r__var__) \
try {\
    operation;\
    __r__var__ = false;\
    std::unique_lock<std::mutex> l(____report_mutex);\
    ____report << __RED__ << "[assertExcept Failed] on operation '" \
               << #operation << "'" << __RESET__ << "\n";\
    l.unlock();\
} catch(...) {\
    __r__var__ = __r__var__ && true;\
}
#define AssertExcept(operation) assertExcept(operation, __r__)

#define RUNTEST int main(){ return (runTestSuite(tests) ? EXIT_SUCCESS : EXIT_FAILURE); }
#define TEST std::map<const std::string, std::function<bool (void)>> tests = {
#define ENDTEST };

#define CASE(name) { name, []() -> bool {\
    bool __r__ = true;\
    try{

#define ENDCASE \
    } catch(const ProtoExcept &e){\
        std::unique_lock<std::mutex> l(____report_mutex);\
        ____report << __RED__ << "Test Case ended on 'ProtoExcept' exception" << std::endl;\
        ____report << __MAG__ << e.what() << __RESET__ << std::endl;\
        l.unlock();\
        return false;\
    } catch(const Except &e){\
        std::unique_lock<std::mutex> l(____report_mutex);\
        ____report << __RED__ << "Test Case ended on 'Except' exception" << std::endl;\
        ____report << __MAG__ << e.what() << __RESET__ << std::endl;\
        l.unlock();\
        return false;\
    }catch(const std::exception &e){\
        std::unique_lock<std::mutex> l(____report_mutex);\
        ____report << __RED__ << "Test Case ended on standard exception" << std::endl;\
        ____report << __MAG__ << e.what() << __RESET__ << std::endl;\
        l.unlock();\
        return false;\
    }catch(...){\
        std::unique_lock<std::mutex> l(____report_mutex);\
        ____report << __RED__ << "Test Case ended on unkown exception" << std::endl;\
        l.unlock();\
        return false;\
    }\
    return __r__; \
}}

// report unequal
template<typename T> 
void reportAssertEqualFailed(T& actual, T& expected, 
    std::ostream& printer(std::ostream&, T&))
{
    std::unique_lock<std::mutex> l(____report_mutex);

    ____report << __RED__ << "[assertEqual Failed]" << __RESET__ << "\n";

    ____report << __GRN__ << "expected : ";
    printer(____report, expected) << "\n";

    ____report << __RED__ << "actual   : ";
    printer(____report, actual  ) << __RESET__ << "\n";
}

// general
template<typename T> 
bool assertEqual
(
    T actual, 
    T expected, 
    
    // implements comparison
    bool comparator(T&, T&) = 
        [](T& a, T& b) -> bool { return a == b; },
    
    // prints object
    std::ostream& printer(std::ostream&, T&) = 
        [](std::ostream& os, T& val) -> std::ostream& { os << val; return os; }
)
{
    if(comparator(actual, expected))
        return true;

    reportAssertEqualFailed(actual, expected, printer);

    return false;
}

// char array
bool assertEqual(const std::string& actual, const std::string& expected, const bool escape = true)
{
    return assertEqual<std::string>(
        escape ? utils::escape(actual) : actual,
        escape ? utils::escape(expected) : expected
    );
}

// char array
bool assertEqual(char *actual, char *expected, const bool escape = true)
{
    if(escape)
    {
        actual = utils::escape(actual);
        expected = utils::escape(expected);
    }

    bool r = assertEqual<char *>(
        escape ? utils::escape(actual) : actual, 
        escape ? utils::escape(expected) : expected,
        [](char *& a, char *& b) -> bool {
            return (strcmp(a, b) == 0);
        });

    if(escape)
    {
        delete [] actual;
        delete [] expected;
    }

    return r;
}

// vector
template<typename E>
bool assertEqual(std::vector<E>& actual, 
                 std::vector<E>& expected)
{
    return assertEqual<std::vector<E>>(actual, expected, 
        [](std::vector<E>& a, std::vector<E>& b) -> bool {
            if (a.size() != b.size())
                return false;

            return std::equal(a.begin(), a.end(), b.begin());
        },
        [](std::ostream& os, std::vector<E>& v) -> std::ostream& { 
            os << std::endl;
            for (E& s : v)
                os << s << std::endl;
            return os;
        });
}

// report not equal failed
template<typename T> 
void reportAssertNotEqualFailed(T& a, T& b, 
    std::ostream& printer(std::ostream&, T&))
{
    std::unique_lock<std::mutex> l(____report_mutex);
    ____report << __RED__ << "[assertNotEqual Failed]" << __RESET__ << "\n";

    ____report << __GRN__ << "a : ";
    printer(____report, a) << "\n";

    ____report << __RED__ << "b : ";
    printer(____report, b  ) << __RESET__ << "\n";
}

// general
template<typename T> 
bool assertNotEqual
(
    T a,
    T b,
    
    // implements comparison
    bool comparator(T&, T&) = 
        [](T& a, T& b) -> bool { return a == b; },
    
    // prints object
    std::ostream& printer(std::ostream&, T&) = 
        [](std::ostream& os, T& val) -> std::ostream& { os << val; return os; }
)
{
    if(!comparator(a, b))
        return true;

    reportAssertNotEqualFailed(a, b, printer);

    return false;
}

// char array
bool assertNotEqual(char *a, char *b)
{
    return assertNotEqual<char *>(a, b, 
        [](char *& a, char *& b) -> bool {
            return (strcmp(a, b) == 0);
        });
}

// vector
template<typename E>
bool assertNotEqual(std::vector<E>& a, 
                    std::vector<E>& b)
{
    return assertNotEqual<std::vector<E>>(a, b, 
        [](std::vector<E>& a, std::vector<E>& b) -> bool {
            if (a.size() != b.size())
                return false;

            return std::equal(a.begin(), a.end(), b.begin());
        },
        [](std::ostream& os, std::vector<E>& v) -> std::ostream& { 
            os << std::endl;
            for (E& s : v)
                os << s << std::endl;
            return os;
        });
}

bool runTest(const std::string& name, std::function<bool (void)> t)
{
    std::cout << __YLW__ << std::setw(55) << std::left << name << __RESET__ << std::flush;

    bool r = t();
    
    std::cout << (r ? (std::string(__GRN__) + "[PASSED]") 
                    : (std::string(__RED__) + "[FAILED]")) 
              << __RESET__ << std::endl;
    
    return r;
}

bool runTestSuite(std::map<const std::string, std::function<bool (void)>> tests)
{
    bool result = true;
    size_t index = 1;
    size_t failed = 0;

    for ( std::pair<const std::string, std::function<bool (void)>> t : tests)
    {
        std::cout << " " << __CYN__ << std::setw(3) << std::left << index++ << " : " << std::flush;
        bool currentTestResult = runTest(t.first, t.second);
        
        if(!currentTestResult)
        {
            std::cout << std::endl << ____report.str() << std::endl;
            failed++;
            if(result) result = false;
        }
    }

    std::cout << " Total  : " << tests.size();
    std::cout << "\tFailed : " << failed << std::endl;

    return result;
}

#endif // __TESTS_H__