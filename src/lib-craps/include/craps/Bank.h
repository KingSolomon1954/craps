//----------------------------------------------------------------
//
// File: Bank.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/MoneyUtil.h>

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
    bool deposit (Gen::Money amount);
    Gen::Money withdraw(Gen::Money amount);
    Bank& operator=(const Bank&) = default;  // assignment
    Bank& operator=(Bank&&)      = default;  // move
    /// @}

    /// @name Observers
    /// @{
    Gen::Money getBalance()      const;
    Gen::Money getAmtDeposited() const;
    Gen::Money getAmtWithdrawn() const;
    /// @}

private:
    Gen::Money refill();

    Gen::Money initialBalance_  = 0;
    Gen::Money refillThreshold_ = 0;
    Gen::Money refillAmount_    = 0;
    Gen::Money amtDeposited_    = 0;
    Gen::Money amtWithdrawn_    = 0;
    Gen::Money amtRefilled_     = 0;
    unsigned numDeposits_       = 0;
    unsigned numWithdrawals_    = 0;
    unsigned numRefills_        = 0;
};

/*-----------------------------------------------------------*//**

@class Bank

@brief Manage money for players and the house.

Manages money.

*/

} // namespace Craps

//----------------------------------------------------------------
