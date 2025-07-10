//----------------------------------------------------------------
//
// File: Bank.cpp
//
//----------------------------------------------------------------

#include "craps/Bank.h"

using namespace Craps;

//----------------------------------------------------------------

Bank::Bank()
{
}

//----------------------------------------------------------------

Bank::Bank(
    Gbl::Money initialBalance,
    Gbl::Money refillThreshold,
    Gbl::Money refillAmount)
    : initialBalance_(initialBalance)
    , refillThreshold_(refillThreshold)
    , refillAmount_(refillAmount)
{

}

//----------------------------------------------------------------

bool
Bank::deposit(Gbl::Money amount)
{
    if (amount <= 0) return false;
    amtDeposited_ += amount;
    return true;
}

//----------------------------------------------------------------

Gbl::Money
Bank::withdraw(Gbl::Money amount)
{
    if (amount <= 0 || amount > getBalance()) return false;
    amtWithdrawn_ += amount;
    return refill();
}

//----------------------------------------------------------------

Gbl::Money
Bank::refill()
{
    if (getBalance() <= refillThreshold_)
    {
        amtRefilled_ += refillAmount_;
        numRefills_++;
        return refillAmount_;
    }
    return 0;
}

//----------------------------------------------------------------

Gbl::Money
Bank::getBalance() const
{
    return initialBalance_ + amtDeposited_ + amtRefilled_ - amtWithdrawn_; 
}

//----------------------------------------------------------------

Gbl::Money
Bank::getAmtDeposited() const
{
    return amtDeposited_;
}

//----------------------------------------------------------------

Gbl::Money
Bank::getAmtWithdrawn() const
{
    return amtWithdrawn_;
}

//----------------------------------------------------------------
