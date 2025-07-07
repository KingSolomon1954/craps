//----------------------------------------------------------------
//
// File: BankStatsStruct.cpp
//
//----------------------------------------------------------------

#include <craps/BankStatsStruct.h>

using namespace Craps;

//-----------------------------------------------------------------

void
BankStats::reset()
{
    balance        = 0;
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
    balance        += session.balance;
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
    node["balance"]        = balance;
    node["numDeposits"]    = numDeposits;
    node["amtDeposited"]   = amtDeposited;
    node["numWithdrawals"] = numWithdrawals;
    node["amtWithdrawn"]   = amtWithdrawn;
    node["numRefills"]     = numRefills;
    node["amtRefilled"]    = amtRefilled;
    
    YAML::Node node2;
    node2["BankStats"] = node;
    return node2;
}

//-----------------------------------------------------------------

void
BankStats::fromYAML(const YAML::Node& node)
{
    if (node["balance"])        balance        = node["balance"].as<unsigned>();
    if (node["numDeposits"])    numDeposits    = node["numDeposits"].as<unsigned>();
    if (node["amtDeposited"])   amtDeposited   = node["amtDeposited"].as<unsigned>();
    if (node["numWithdrawals"]) numWithdrawals = node["numWithdrawals"].as<unsigned>();
    if (node["amtWithdrawn"])   amtWithdrawn   = node["amtWithdrawn"].as<unsigned>();
    if (node["numRefills"])     numRefills     = node["numRefills"].as<unsigned>();
    if (node["amtRefilled"])    amtRefilled    = node["amtRefilled"].as<unsigned>();
}

//-----------------------------------------------------------------
