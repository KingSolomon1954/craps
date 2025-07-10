//----------------------------------------------------------------
//
// File: BankStats.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/Globals.h>
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
        
    void reset();
    void merge(const BankStats& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BankStats&) const = default;  // Only generates ==
};

} // namespace Craps

//----------------------------------------------------------------
