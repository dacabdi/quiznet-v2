#ifndef __UNIFORMRANDOM__H__
#define __UNIFORMRANDOM__H__

#include "Common.h"

#include <random>
#include <limits>

template<typename T>
class UniformRandom
{
    public:

        UniformRandom(T min = std::numeric_limits<T>::min(),
                      T max = std::numeric_limits<T>::max());

        T generate(T min, T max);
        T generate(void);
        void setMin(const T min);
        void setMax(const T max);

    private:

        std::mt19937 _mt;

        T _min;
        T _max;
};

#endif // __UNIFORMRANDOM__H__