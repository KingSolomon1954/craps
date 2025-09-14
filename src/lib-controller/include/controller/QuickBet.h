//----------------------------------------------------------------
//
// File: QuickBet.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>

namespace Ctrl {

class QuickBet
{
public:
    struct QuickBetEntry
    {
        BetName    betName    = BetName::Invalid;
        size_t     pivot      = 0;
        bool       isOddsBet  = false;
        Gen::Money amount     = 0;
    };
    using Bets = std::vector<QuickBetEntry>;
    
    /// @name Lifecycle
    /// @{
    /// @}
    
    /// @name Modifiers
    /// @{
    void applyQuickBet (size_t index);
    void setQuickBet   (const QuickBetEntry& afe);
    void deleteQuickBet(const QuickBetEntry& afe);
    void deleteQuickBet(size_t index);
    /// @}

    /// @name Observers
    /// @{
    const Bets& getQuickBets();
    /// @}
    
private:
    Bets bets_;
};

/*-----------------------------------------------------------*//**

@class QuickBet

@brief Manages automatic bets

The QuickBet class in controller has these responsibilities:

@li Maintains a list of quick bets in a std::vector

@li applyQuickBet() - makes the bet
@li setQuickBet() - replaces a quick bet if the entry already exists, zero is valid amount
@li getQuickBets() - returns the read-only vector of quick bet entries
@li deleteQuickBet() - delete an entry, shift entries if necessary, no empty slots

*/

} // namespace Ctrl

//----------------------------------------------------------------
