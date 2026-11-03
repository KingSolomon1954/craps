//----------------------------------------------------------------
//
// File: LastFill.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <gen/ReturnCode.h>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class LastFill
{
public:
    struct LastFillEntry
    {
        BetName    betName    = BetName::Invalid;
        size_t     pivot      = 0;
        bool       isOddsBet  = false;
        Gen::Money amount     = 0;
    };
    using LastFills = std::vector<LastFillEntry>;
    
    /// @name Lifecycle
    /// @{
    LastFill() = default;
   ~LastFill() = default;
    static LastFill& instance();
    /// @}
    
    /// @name Modifiers
    /// @{
    void setLastFill   (const LastFillEntry& lfe);
    void deleteLastFill(const LastFillEntry& lfe);
    void clear();
    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    Gen::ReturnCode getLastFill(LastFillEntry& lfe);
    const LastFills& getLastFills();
    YAML::Node toYAML() const;
    /// @}
    
private:
    LastFills fills_;

    // Helper to find an entry by betName/pivot/isOddsBet
    LastFills::iterator       findEntry(const LastFillEntry& lfe);
    LastFills::const_iterator findEntry(const LastFillEntry& lfe) const;
};

/*-----------------------------------------------------------*//**

@class LastFill

@brief Manages last fill amounts for bets

The LastFill class in controller has these responsibilities:

@li remembers the last amount used by a particular bet type in std::vector

@li setLastFill() - replaces amount if the entry already exists
@li getLastFill() - returns the amount used last time for a particular bet
@li getLastFills() - returns the read-only vector of last fill entries
@li deleteLastFill() - shift entries if necessary, no empty slots
@li clear() - forgets all entries, e.g. before loading another player's

@par Persistence

toYAML() and fromYAML() convert between the entries and a YAML
sequence. The class does not touch files itself; PlayerManager stores
the sequence under "LastFills" in the user Player's "BetPreferences"
section and the Player writes it to its file. Example:

@code
BetPreferences:
  LastFills:
    - betName: PassLine
      pivot: 0
      isOddsBet: false
      amount: 5000
@endcode

amount is Gen::Money's own YAML form. fromYAML() replaces the current
contents and is forgiving: a missing or malformed section, or an
individual bad entry, is logged and skipped so a hand-edited player
file can never stop the game from starting.

*/

} // namespace Ctrl

//----------------------------------------------------------------
