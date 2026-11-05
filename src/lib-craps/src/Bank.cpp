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
    Gen::Money sessionStartBalance,
    Gen::Money refillThreshold,
    Gen::Money refillAmount)
    : sessionStartBalance_(sessionStartBalance)
    , refillThreshold_(refillThreshold)
    , refillAmount_(refillAmount)
{

}

//----------------------------------------------------------------

void
Bank::deposit(Gen::Money amount)
{
    if (amount == 0) return;
    
    amtDeposited_ += amount;
    numDeposits_++;
    currentStats_.amtDeposited = amtDeposited_;
    currentStats_.numDeposits = numDeposits_;
}

//----------------------------------------------------------------

bool
Bank::withdraw(Gen::Money amount)
{
    if (amount == 0) return false;
    
    amtWithdrawn_ += amount;
    numWithdrawals_++;
    currentStats_.amtWithdrawn = amtWithdrawn_;
    currentStats_.numWithdrawals = numWithdrawals_;
    
    return refill();
}

//----------------------------------------------------------------

bool
Bank::refill()
{
    if (getBalance() <= refillThreshold_)
    {
        amtRefilled_ += refillAmount_;
        numRefills_++;
        currentStats_.amtRefilled = amtRefilled_;
        currentStats_.numRefills = numRefills_;
        return true;
    }
    return false;
}

//----------------------------------------------------------------

Gen::Money
Bank::getBalance() const
{
    return sessionStartBalance_ + amtDeposited_ + amtRefilled_ - amtWithdrawn_; 
}

//----------------------------------------------------------------

int
Bank::getSessionNet() const
{
    return amtDeposited_ - amtWithdrawn_; 
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

//-----------------------------------------------------------------

void
Bank::mergeStats()
{
    alltimeStats_.merge(currentStats_);
}

//-----------------------------------------------------------------

void
Bank::resetStats()
{
    currentStats_.reset();
}

//-----------------------------------------------------------------
//
// Starts a new session. Whatever the balance is now becomes the
// session's starting balance, and everything counted for the previous
// session is cleared. The balance itself does not change. Call after
// mergeStats(), or the previous session's stats are lost.
//
void
Bank::beginNewSession()
{
    sessionStartBalance_ = getBalance();
    amtDeposited_        = 0;
    amtWithdrawn_        = 0;
    amtRefilled_         = 0;
    numDeposits_         = 0;
    numWithdrawals_      = 0;
    numRefills_          = 0;
    currentStats_.reset();
}

//-----------------------------------------------------------------

const BankStats&
Bank::getCurrentStats() const
{
    return currentStats_;
}

//-----------------------------------------------------------------

const BankStats&
Bank::getAlltimeStats() const
{
    return alltimeStats_;
}

//-----------------------------------------------------------------

YAML::Node
Bank::toYAML() const
{
    YAML::Node node;
    node["originalStartBalance"] = originalStartBalance_;
    node["sessionStartBalance"]  = getBalance();
    node["refillThreshold"]      = refillThreshold_;
    node["refillAmount"]         = refillAmount_;
    node["bankStats"]            = alltimeStats_.toYAML();
    return node;
}

//-----------------------------------------------------------------

void
Bank::fromYAML(const YAML::Node& node)
{
    originalStartBalance_ = node["originalStartBalance"].as<Gen::Money>();
    sessionStartBalance_  = node["sessionStartBalance"].as<Gen::Money>();
    refillThreshold_      = node["refillThreshold"].as<Gen::Money>();
    refillAmount_         = node["refillAmount"].as<Gen::Money>();

    alltimeStats_.fromYAML(node["bankStats"]);
}

//----------------------------------------------------------------
