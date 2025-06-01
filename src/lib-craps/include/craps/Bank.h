//----------------------------------------------------------------
//
// File: Bank.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <optional>
#include <controller/Globals.h>

namespace Craps {

class Bank
{
public:
    enum class TransactionType { Deposit, Withdraw, Refill };

//  enum class TransactionType { StartSession, EndSession, Refill };

    struct Transaction
    {
        TransactionType type;
        Gbl::Money amount;
        std::string note;
    };

    /// @name Lifecycle
    /// @{
    Bank(Gbl::Money initialBalance = 0);
    /// @}

    /// @name Modifiers
    /// @{
    bool deposit (Gbl::Money amount, const std::string& note = "");
    bool withdraw(Gbl::Money amount, const std::string& note = "");
    void refill  (Gbl::Money amount, const std::string& note = "Refill");
    /// @}

    /// @name Observers
    /// @{
    Gbl::Money getBalance() const;
    const std::vector<Transaction>& getHistory() const;
    Gbl::Money totalDeposited() const;
    Gbl::Money totalWithdrawn() const;
    /// @}

private:
    Gbl::Money balance_ = 0;
    std::vector<Transaction> history_;
};

/*-----------------------------------------------------------*//**

@class Bank

@brief Manage money for players and the house.

Manages money.

*/

} // namespace Craps

//----------------------------------------------------------------
