//----------------------------------------------------------------
//
// File: BankStats.cpp
//
//----------------------------------------------------------------

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
    return node;
}

//-----------------------------------------------------------------

void
BankStats::fromYAML(const YAML::Node& node)
{
    if (node["initialStartingBalance"]) initialStartingBalance = node["initialStartingBalance"].as<unsigned>();
    if (node["numDeposits"])            numDeposits            = node["numDeposits"].as<unsigned>();
    if (node["amtDeposited"])           amtDeposited           = node["amtDeposited"].as<unsigned>();
    if (node["numWithdrawals"])         numWithdrawals         = node["numWithdrawals"].as<unsigned>();
    if (node["amtWithdrawn"])           amtWithdrawn           = node["amtWithdrawn"].as<unsigned>();
    if (node["numRefills"])             numRefills             = node["numRefills"].as<unsigned>();
    if (node["amtRefilled"])            amtRefilled            = node["amtRefilled"].as<unsigned>();
}

//-----------------------------------------------------------------
