#include "UniformRandom.h"

template<typename T>
UniformRandom<T>::UniformRandom(T min, T max)
: _min(min), _max(max)
{
    std::random_device rd; 
    _mt = std::mt19937(rd()); 
    
}

template<typename T>
T UniformRandom<T>::generate(void)
{
    std::uniform_int_distribution<T> dist(_min, _max);
    return dist(_mt);
}

template<typename T>
T UniformRandom<T>::generate(T min, T max)
{
    std::uniform_int_distribution<T> dist(min, max);
    T value = dist(_mt);
    return value;
}

template<typename T>
void UniformRandom<T>::setMin(const T min)
{
    _min = min;
}

template<typename T>
void UniformRandom<T>::setMax(const T max)
{
    _max = max;
}

// integral types :
//      short, int, long, long long, unsigned short, 
//      unsigned int, unsigned long, unsigned long long
template class UniformRandom<short>;
template class UniformRandom<int>;
template class UniformRandom<long>;
template class UniformRandom<long long>;
template class UniformRandom<unsigned short>;
template class UniformRandom<unsigned char>;
template class UniformRandom<unsigned int>;
template class UniformRandom<unsigned long>;
template class UniformRandom<unsigned long long>;