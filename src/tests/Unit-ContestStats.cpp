#include "ContestStats.h"
#include "Test.h"

TEST

    CASE("simpleStats")

        // mock data set
        std::map<uint32_t, uint32_t> set;
        
        // five questions
        set.emplace(1,3); // 0.6
        set.emplace(2,2); // 0.4
        set.emplace(3,0); // 0.0
        set.emplace(4,1); // 0.2
        set.emplace(5,4); // 0.8

        std::map<uint32_t, double> expPercentages;
        expPercentages.emplace(1, 0.6);
        expPercentages.emplace(2, 0.4);
        expPercentages.emplace(3, 0.0);
        expPercentages.emplace(4, 0.2);
        expPercentages.emplace(5, 0.8);

        // expected serialization
        std::string expSer = 
        "5\n"     // number of contestants
        "3\n"     // highest score
        "5\n"     // number of questions
        "1 3\n"   // q1id -> q1correct
        "2 2\n"   // ...
        "3 0\n"
        "4 1\n"
        "5 4\n";  // qnid -> qncorrect

        // expected stats
        double expAverage = 0.4;
        uint32_t expMax = 4;
        uint32_t contestants = 5;
        uint32_t highest = 3;

        // create object
        ContestStats stat(set, contestants, highest);

        // assertions
        AssertEqual(stat.average(), expAverage);
        AssertEqual(stat.max(), expMax);
        AssertEqual(stat.set(), set);
        AssertEqual(stat.contestants(), contestants);
        AssertEqual(stat.highest(), highest)
        for(const auto &avg : expPercentages) 
            AssertEqual(stat.percentage(avg.first), avg.second);
        AssertEqual(stat.serialize(), expSer);

    ENDCASE,

    CASE("emptyStats")

        // mock data set
        std::map<uint32_t, uint32_t> set;
        std::map<uint32_t, double> expPercentages;

        // expected serialization
        std::string expSer =
        "0\n"
        "0\n"
        "0\n";

        // expected stats
        double expAverage = 0.;
        uint32_t expMax = 0;
        uint32_t contestants = 0;
        uint32_t highest = 0;

        // create object
        ContestStats stat(set, contestants, highest);

        // assertions
        AssertEqual(stat.average(), expAverage);
        AssertEqual(stat.max(), expMax);
        AssertEqual(stat.set(), set);
        AssertEqual(stat.contestants(), contestants);
        AssertEqual(stat.highest(), highest)
        for(const auto &avg : expPercentages) 
            AssertEqual(stat.percentage(avg.first), avg.second);
        AssertEqual(stat.serialize(), expSer);

    ENDCASE,

    CASE("deserializeNormalSet")

        // mock data set
        std::map<uint32_t, uint32_t> set;
        
        // five questions
        set.emplace(1,3); // 0.6
        set.emplace(2,2); // 0.4
        set.emplace(3,0); // 0.0
        set.emplace(4,1); // 0.2
        set.emplace(5,4); // 0.8

        std::map<uint32_t, double> expPercentages;
        expPercentages.emplace(1, 0.6);
        expPercentages.emplace(2, 0.4);
        expPercentages.emplace(3, 0.0);
        expPercentages.emplace(4, 0.2);
        expPercentages.emplace(5, 0.8);

        // expected serialization
        std::string expSer =
        "5\n"     // number of contestants
        "3\n"     // highest score
        "5\n"     // number of questions
        "1 3\n"   // q1id -> q1correct
        "2 2\n"   // ...
        "3 0\n"
        "4 1\n"
        "5 4\n";  // qnid -> qncorrect

        // expected stats
        double expAverage = 0.4;
        uint32_t expMax = 4;

        uint32_t contestants = 5;
        uint32_t highest = 3;

        // create object
        ContestStats stat(expSer);

        // assertions
        AssertEqual(stat.average(), expAverage);
        AssertEqual(stat.max(), expMax);
        AssertEqual(stat.set(), set);
        AssertEqual(stat.contestants(), contestants);
        AssertEqual(stat.highest(), highest)
        for(const auto &avg : expPercentages) 
            AssertEqual((stat.percentage(avg.first)), avg.second);
        AssertEqual(stat.serialize(), expSer);

    ENDCASE,

    CASE("emptyStats")

        // mock data set
        std::map<uint32_t, uint32_t> set;
        std::map<uint32_t, double> expPercentages;

        // expected serialization
        std::string expSer =
        "0\n"
        "0\n"
        "0\n";

        // expected stats
        double expAverage = 0.;
        uint32_t expMax = 0;
        uint32_t contestants = 0;
        uint32_t highest = 0;

        // create object
        ContestStats stat(expSer);

        // assertions
        AssertEqual(stat.average(), expAverage);
        AssertEqual(stat.max(), expMax);
        AssertEqual(stat.set(), set);
        AssertEqual(stat.contestants(), contestants);
        AssertEqual(stat.highest(), highest)
        for(const auto &avg : expPercentages) 
            AssertEqual(stat.percentage(avg.first), avg.second);
        AssertEqual(stat.serialize(), expSer);

    ENDCASE

ENDTEST

RUNTEST