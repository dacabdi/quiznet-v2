#ifndef __ROUNDRESULTS__H__
#define __ROUNDRESULTS__H__

#include "ProtoExcept.h"

#include "Common.h"

#include <cstdint>
#include <cmath>

class RoundResults
{
    public:

        RoundResults(void);
        RoundResults(const std::string& str);
        RoundResults(bool correct, double ratio, uint32_t questions, uint32_t score, uint32_t max);

        static RoundResults deserialize(const std::string& str);
        std::string serialize(void) const;

        bool     correct(void) const;
        double   ratio(void) const;
        uint32_t questions(void) const;
        uint32_t score(void) const;
        uint32_t max(void) const;

        std::string present(void) const;

        friend std::ostream& operator<<(std::ostream &os, const RoundResults& ref);
        bool operator==(const RoundResults& ref) const;
        bool operator!=(const RoundResults& ref) const;

    private:

        bool     _correct = false;
        double   _ratio = 0.;
        uint32_t _questions = 0;
        uint32_t _score = 0;
        uint32_t _max = 0;
};

#endif // __ROUNDRESULTS__H__