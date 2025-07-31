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
    amtDeposited_ += amount;
    numDeposits_++;
    currentStats_.amtDeposited = amtDeposited_;
    currentStats_.numDeposits = numDeposits_;
}

//----------------------------------------------------------------

bool
Bank::withdraw(Gen::Money amount)
{
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
