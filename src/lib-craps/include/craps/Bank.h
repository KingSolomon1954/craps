//----------------------------------------------------------------
//
// File: Bank.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/Globals.h>
#include <craps/BankStatsStruct.h>

namespace Craps {

class Bank
{
public:
    /// @name Lifecycle
    /// @{
    Bank();
    Bank(Gbl::Money initialBalance,
         Gbl::Money refillThreshold,
         Gbl::Money refillAmount);
    Bank(const Bank&) = default;  // copy ctor
    /// @}

    /// @name Modifiers
    /// @{
    bool deposit (Gbl::Money amount);
    bool withdraw(Gbl::Money amount);
    Bank& operator=(const Bank&) = default;  // assignment
    Bank& operator=(Bank&&)      = default;  // move
    /// @}

    /// @name Observers
    /// @{
    Gbl::Money getBalance()      const;
    Gbl::Money getAmtDeposited() const;
    Gbl::Money getAmtWithdrawn() const;
    BankStats  getSessionStats() const;
    BankStats  getAlltimeStats() const;
    /// @}

private:
    void refill();

    Gbl::Money initialBalance_  = 0;
    Gbl::Money refillThreshold_ = 0;
    Gbl::Money refillAmount_    = 0;
    Gbl::Money amtDeposited_          = 0;
    Gbl::Money amtWithdrawn_          = 0;
    Gbl::Money amtRefilled_           = 0;
    unsigned numDeposits_             = 0;
    unsigned numWithdrawals_          = 0;
    unsigned numRefills_              = 0;
    
    BankStats sessionStats_;
    BankStats alltimeStats_;
};

/*-----------------------------------------------------------*//**

@class Bank

@brief Manage money for players and the house.

Manages money.

*/

} // namespace Craps

//----------------------------------------------------------------
