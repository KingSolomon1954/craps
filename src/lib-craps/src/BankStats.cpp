//----------------------------------------------------------------
//
// File: BankStats.cpp
//
//----------------------------------------------------------------

#include <string>
#include <craps/BankStats.h>

using namespace Craps;

//-----------------------------------------------------------------

void
BankStats::reset()
{
    // initialStartingBalance // does not get reset
    numDeposits    = 0;
    amtDeposited   = 0;
    numWithdrawals = 0;
    amtWithdrawn   = 0;
    numRefills     = 0;
    amtRefilled    = 0;
    // maxAmtDepositedSession = 0;  // does not get reset
    // maxAmtWithdrawnSession = 0;  // does not get reset
}

//-----------------------------------------------------------------

void
BankStats::merge(const BankStats& session)
{
    // initialStartingBalance // does not get merged
    numDeposits    += session.numDeposits;
    amtDeposited   += session.amtDeposited;
    numWithdrawals += session.numWithdrawals;
    amtWithdrawn   += session.amtWithdrawn;
    numRefills     += session.numRefills;
    amtRefilled    += session.amtRefilled;

    if (session.amtDeposited > maxAmtDepositedSession)
    {
        maxAmtDepositedSession = session.amtDeposited;
        maxAmtDepositedSessionDate.setToNow();
    }
    if (session.amtWithdrawn > maxAmtWithdrawnSession)
    {
        maxAmtWithdrawnSession = session.amtWithdrawn;
        maxAmtWithdrawnSessionDate.setToNow();
    }
}

//-----------------------------------------------------------------

YAML::Node 
BankStats::toYAML() const
{
    YAML::Node node;
    node["initialStartingBalance"] = initialStartingBalance;
    node["numDeposits"]            = numDeposits;
    node["amtDeposited"]           = amtDeposited;
    node["numWithdrawals"]         = numWithdrawals;
    node["amtWithdrawn"]           = amtWithdrawn;
    node["numRefills"]             = numRefills;
    node["amtRefilled"]            = amtRefilled;
    node["maxAmtDepositedSession"] = maxAmtDepositedSession;
    node["maxAmtWithdrawnSession"] = maxAmtWithdrawnSession;
    node["maxAmtDepositedSessionDate"] = maxAmtWithdrawnSessionDate.toString();
    node["maxAmtWithdrawnSessionDate"] = maxAmtWithdrawnSessionDate.toString();
    return node;
}

//-----------------------------------------------------------------

void
BankStats::fromYAML(const YAML::Node& node)
{
    if (node["initialStartingBalance"])     initialStartingBalance     = node["initialStartingBalance"].as<unsigned>();
    if (node["numDeposits"])                numDeposits                = node["numDeposits"].as<unsigned>();
    if (node["amtDeposited"])               amtDeposited               = node["amtDeposited"].as<unsigned>();
    if (node["numWithdrawals"])             numWithdrawals             = node["numWithdrawals"].as<unsigned>();
    if (node["amtWithdrawn"])               amtWithdrawn               = node["amtWithdrawn"].as<unsigned>();
    if (node["numRefills"])                 numRefills                 = node["numRefills"].as<unsigned>();
    if (node["amtRefilled"])                amtRefilled                = node["amtRefilled"].as<unsigned>();
    if (node["maxAmtDepositedSession"])     maxAmtDepositedSession     = node["maxAmtDepositedSession"].as<unsigned>();
    if (node["maxAmtWithdrawnSession"])     maxAmtWithdrawnSession     = node["maxAmtWithdrawnSession"].as<unsigned>();
    if (node["maxAmtDepositedSessionDate"]) maxAmtDepositedSessionDate = node["maxAmtDepositedSessionDate"].as<std::string>();
    if (node["maxAmtWithdrawnSessionDate"]) maxAmtWithdrawnSessionDate = node["maxAmtWithdrawnSessionDate"].as<std::string>();
}

//-----------------------------------------------------------------
