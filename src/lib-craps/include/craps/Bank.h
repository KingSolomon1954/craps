//----------------------------------------------------------------
//
// File: Bank.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/MoneyUtil.h>
#include <craps/BankStats.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

class Bank
{
public:
    /// @name Lifecycle
    /// @{
    Bank();
    Bank(Gen::Money initialBalance,
         Gen::Money refillThreshold,
         Gen::Money refillAmount);
    Bank(const Bank&) = default;  // copy ctor
    /// @}

    /// @name Modifiers
    /// @{
    void deposit (Gen::Money amount);
    bool withdraw(Gen::Money amount);
    void mergeStats();
    Bank& operator=(const Bank&) = default;  // assignment
    Bank& operator=(Bank&&)      = default;  // move
    /// @}

    /// @name Observers
    /// @{
    Gen::Money getBalance()      const;
    Gen::Money getAmtDeposited() const;
    Gen::Money getAmtWithdrawn() const;
    /// @}

    // @name YAML operations
    /// @{
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    /// @}
    
private:
    bool refill();

    Gen::Money initialStartingBalance_ = 0;
    Gen::Money initialBalance_         = 0;
    Gen::Money refillThreshold_        = 0;
    Gen::Money refillAmount_           = 0;
    Gen::Money amtDeposited_           = 0;
    Gen::Money amtWithdrawn_           = 0;
    Gen::Money amtRefilled_            = 0;
    unsigned numDeposits_              = 0;
    unsigned numWithdrawals_           = 0;
    unsigned numRefills_               = 0;
    BankStats currentStats_;
    BankStats alltimeStats_;
};

/*-----------------------------------------------------------*//**

@class Bank

@brief Manage money for players and the house.

Manages money.

*/

} // namespace Craps

//----------------------------------------------------------------
