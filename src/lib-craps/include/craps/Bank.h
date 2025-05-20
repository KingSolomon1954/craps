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
#include "craps/Globals.h"

namespace Craps {

class Bank
{
public:
    enum class TransactionType { Deposit, Withdraw, Refill };

//  enum class TransactionType { StartSession, EndSession, Refill };

    struct Transaction
    {
        TransactionType type;
        Money amount;
        std::string note;
    };

    /// @name Lifecycle
    /// @{
    Bank(Money initialBalance = 0);
    /// @}

    /// @name Modifiers
    /// @{
    bool deposit (Money amount, const std::string& note = "");
    bool withdraw(Money amount, const std::string& note = "");
    void refill  (Money amount, const std::string& note = "Refill");
    /// @}

    /// @name Observers
    /// @{
    Money getBalance() const;
    const std::vector<Transaction>& getHistory() const;
    Money totalDeposited() const;
    Money totalWithdrawn() const;
    /// @}

private:
    Money balance_ = 0;
    std::vector<Transaction> history_;
};

/*-----------------------------------------------------------*//**

@class Bank

@brief Manage money for players and the house.

Manages money.

*/

} // namespace Craps

//----------------------------------------------------------------
