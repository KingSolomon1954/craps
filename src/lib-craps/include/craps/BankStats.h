//----------------------------------------------------------------
//
// File: BankStats.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/MoneyUtil.h>
#include <gen/Timepoint.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

//----------------------------------------------------------------

class BankStats
{
public:
    Gen::Money initialStartingBalance = 0;
    unsigned   numDeposits            = 0;
    Gen::Money amtDeposited           = 0;
    unsigned   numWithdrawals         = 0;
    Gen::Money amtWithdrawn           = 0;
    unsigned   numRefills             = 0;
    Gen::Money amtRefilled            = 0;

    Gen::Money maxAmtDepositedSession = 0;
    Gen::Money maxAmtWithdrawnSession = 0;
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
