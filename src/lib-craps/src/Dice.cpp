//----------------------------------------------------------------
//
// File: Dice.cpp
//
//----------------------------------------------------------------

#include "craps/Dice.h"
#include <iostream>
#include <random>
#include <stdexcept>

using namespace Craps;

/*-----------------------------------------------------------*//**

Constructor

*/
Dice::Dice()
{
    // Empty.
}

/*-----------------------------------------------------------*//**

*/
unsigned
Dice::roll()
{
    d1_ = getRandomNumber(1, 6);
    d2_ = getRandomNumber(1, 6);
    return d1_ + d2_;
}

/*-----------------------------------------------------------*//**

*/
void
Dice::set(unsigned d1, unsigned d2)
{
    unsigned total = d1 + d2;
    if ((total < 2) || (total > 12 ))
    {
        throw std::invalid_argument("Bad dice value:" + std::to_string(total));
    }
    d1_ = d1;
    d2_ = d2;
}

/*-----------------------------------------------------------*//**

*/
unsigned
Dice::value() const
{
    return d1_ + d2_;
}

/*-----------------------------------------------------------*//**

*/
unsigned
Dice::d1() const
{
    return d1_;
}

/*-----------------------------------------------------------*//**

*/
unsigned
Dice::d2() const
{
    return d2_;
}

//----------------------------------------------------------------

int
Dice::getRandomNumber(int min, int max) const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

//----------------------------------------------------------------
