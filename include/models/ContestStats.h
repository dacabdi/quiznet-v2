#ifndef __CONTEST_STATS_H__
#define __CONTEST_STATS_H__

#include "Except.h"
#include "Common.h"
#include "Utils.h"

#include <sstream>
#include <cstdint>
#include <map>

using score_set      = std::map<uint32_t, uint32_t>;
using percentage_set = std::map<uint32_t, double>  ;

class ContestStats
{
    public:
        
        ContestStats(void);
        ContestStats(score_set, const uint32_t, const uint32_t);
        ContestStats(const std::string&);

        static ContestStats deserialize(const std::string&);
        static ContestStats deserialize(std::istream& iss);
        std::string serialize(void) const;

        uint32_t weight(void) const;            // get the weight of this data set (|contestants| * |questions|)
        double average(void) const;             // get the average across questions
        uint32_t max(void) const;               // get the max score per question
        uint32_t highest(void) const;           // get the highest score per player
        score_set set(void) const;              // get a copy of he score set
        uint32_t contestants(void) const;       // get the number of contestants
        size_t questions(void) const;           // get the number of questions
        
        double percentage(const uint32_t) const;    // get a question's percentage of correct answers
        percentage_set percentage(void) const;      // get a set of percentages

        friend std::ostream& operator<<(std::ostream &os, const ContestStats& ref);
        bool operator==(const ContestStats& ref) const;
        bool operator!=(const ContestStats& ref) const;
        
    private:
        
        score_set _set;
        uint32_t  _contestants = 0;
        uint32_t  _highest = 0;

        double _percentage(const uint32_t) const;  // returns percentage for score 
};

#endif // __CONTEST_STATS_H__