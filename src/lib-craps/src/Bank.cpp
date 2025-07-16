//----------------------------------------------------------------
//
// File: Bank.cpp
//
//----------------------------------------------------------------

#include <craps/Bank.h>

using namespace Craps;

//----------------------------------------------------------------

Bank::Bank()
{
}

//----------------------------------------------------------------

Bank::Bank(
    Gen::Money initialBalance,
    Gen::Money refillThreshold,
    Gen::Money refillAmount)
    : initialBalance_(initialBalance)
    , refillThreshold_(refillThreshold)
    , refillAmount_(refillAmount)
{

}

//----------------------------------------------------------------

bool
Bank::deposit(Gen::Money amount)
{
    if (amount <= 0) return false;
    amtDeposited_ += amount;
    return true;
}

//----------------------------------------------------------------

Gen::Money
Bank::withdraw(Gen::Money amount)
{
    if (amount <= 0 || amount > getBalance()) return false;
    amtWithdrawn_ += amount;
    return refill();
}

//----------------------------------------------------------------

Gen::Money
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

Gen::Money
Bank::getBalance() const
{
    return initialBalance_ + amtDeposited_ + amtRefilled_ - amtWithdrawn_; 
}

//----------------------------------------------------------------

Gen::Money
Bank::getAmtDeposited() const
{
    return amtDeposited_;
}

//----------------------------------------------------------------

Gen::Money
Bank::getAmtWithdrawn() const
{
    return amtWithdrawn_;
}

//----------------------------------------------------------------
