//----------------------------------------------------------------
//
// File: Dice.cpp
//
//----------------------------------------------------------------

#include "Dice.h"
#include <iostream>
#include <random>

using namespace App;

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
