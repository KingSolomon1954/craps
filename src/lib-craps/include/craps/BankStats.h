//----------------------------------------------------------------
//
// File: BankStats.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/Globals.h>
#include <gen/Timepoint.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

//----------------------------------------------------------------

class BankStats
{
public:
    Gbl::Money initialStartingBalance = 0;
    unsigned   numDeposits    = 0;
    Gbl::Money amtDeposited   = 0;
    unsigned   numWithdrawals = 0;
    Gbl::Money amtWithdrawn   = 0;
    unsigned   numRefills     = 0;
    Gbl::Money amtRefilled    = 0;

    Gbl::Money maxAmtDepositedSession = 0;
    Gbl::Money maxAmtWithdrawnSession = 0;
    Gen::Timepoint maxAmtDepositedSessionDate;
    Gen::Timepoint maxAmtWithdrawnSessionDate;
    
    void reset();
    void merge(const BankStats& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BankStats&) const = default;  // Only generates ==
};

} // namespace Craps

//----------------------------------------------------------------
