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

bool
Bank::withdraw(Gbl::Money amount)
{
    if (amount <= 0 || amount > getBalance()) return false;
    amtWithdrawn_ += amount;
    refill();
    return true;

    sessionStats_.amtWithdrawn += amount;

}

//----------------------------------------------------------------

void
Bank::refill()
{
    if (getBalance() <= refillThreshold_)
    {
        amtRefilled_ += refillAmount_;
        numRefills_++;
    }
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

BankStats
Bank::getSessionStats() const
{
    return sessionStats_;
}

//----------------------------------------------------------------

BankStats
Bank::getAlltimeStats() const
{
    return alltimeStats_;
}

//----------------------------------------------------------------
