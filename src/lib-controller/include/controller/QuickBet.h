//----------------------------------------------------------------
//
// File: QuickBet.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <yaml-cpp/yaml.h>

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
    using QuickBets = std::vector<QuickBetEntry>;
    
    /// @name Lifecycle
    /// @{
    QuickBet() = default;
   ~QuickBet() = default;
    /// @}
    
    /// @name Modifiers
    /// @{
    void applyQuickBet (size_t index);
    void setQuickBet   (const QuickBetEntry& qbe);
    void deleteQuickBet(const QuickBetEntry& qbe);
    void deleteQuickBet(size_t index);
    void clear();
    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    const QuickBets& getQuickBets();
    YAML::Node toYAML() const;
    /// @}
    
private:
    QuickBets bets_;
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
@li clear() - forgets all entries

@par Ownership and persistence

Not a singleton, and it publishes no events; it is plain data. Each
player has one, held in PlayerPrefs, which PlayerManager owns.
EventHandler changes the user's QuickBet and publishes UslQuickBetChanged.

The order of the entries matters, since quick bets are applied and
deleted by index. toYAML() and fromYAML() keep it. They convert between
the entries and a YAML sequence (see BetEntryYaml.h). PlayerPrefs puts
it under "QuickBets" in the player file's "Prefs" section. fromYAML()
replaces the current contents and never throws; bad entries are logged
and skipped.

*/

} // namespace Ctrl

//----------------------------------------------------------------
