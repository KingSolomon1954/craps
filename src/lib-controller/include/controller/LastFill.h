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

@par Ownership and persistence

Not a singleton. Each player has one, held in PlayerPrefs, which
PlayerManager owns. The View and EventHandler reach the right one
through PlayerManager.

toYAML() and fromYAML() convert between the entries and a YAML
sequence. The class does not touch files; PlayerPrefs puts the sequence
under "LastFills" in the player file's "Prefs" section. fromYAML()
replaces the current contents and is forgiving: a missing or malformed
sequence, or an individual bad entry, is logged and skipped so a
hand-edited player file can never stop the game from starting.

*/

} // namespace Ctrl

//----------------------------------------------------------------
