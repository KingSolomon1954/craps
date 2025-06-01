//----------------------------------------------------------------
//
// File: Bank.cpp
//
//----------------------------------------------------------------

#include "craps/Bank.h"

using namespace Craps;

//----------------------------------------------------------------

Bank::Bank(Gbl::Money initialBalance)
    : balance_(initialBalance)
{
    history_.push_back({ TransactionType::Refill, initialBalance, "Initial Balance" });
}

//----------------------------------------------------------------

bool
Bank::deposit(Gbl::Money amount, const std::string& note)
{
    if (amount <= 0) return false;
    balance_ += amount;
    history_.push_back({ TransactionType::Deposit, amount, note });
    return true;
}

//----------------------------------------------------------------

bool
Bank::withdraw(Gbl::Money amount, const std::string& note)
{
    if (amount <= 0 || amount > balance_) return false;
    balance_ -= amount;
    history_.push_back({ TransactionType::Withdraw, amount, note });
    return true;
}

//----------------------------------------------------------------

void
Bank::refill(Gbl::Money amount, const std::string& note)
{
    if (amount <= 0) return;
    balance_ += amount;
    history_.push_back({ TransactionType::Refill, amount, note });
}

//----------------------------------------------------------------

Gbl::Money
Bank::getBalance() const
{
    return balance_;
}

//----------------------------------------------------------------

const std::vector<Bank::Transaction>&
Bank::getHistory() const
{
    return history_;
}

//----------------------------------------------------------------

Gbl::Money
Bank::totalDeposited() const
{
    Gbl::Money total = 0;
    for (const auto& tx : history_)
    {
        if (tx.type == TransactionType::Deposit)
        {
            total += tx.amount;
        }
    }
    return total;
}

//----------------------------------------------------------------

Gbl::Money
Bank::totalWithdrawn() const
{
    Gbl::Money total = 0;
    for (const auto& tx : history_)
    {
        if (tx.type == TransactionType::Withdraw)
        {
            total += tx.amount;
        }
    }
    return total;
}

//----------------------------------------------------------------

