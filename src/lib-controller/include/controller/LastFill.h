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
    /// @}

    /// @name Observers
    /// @{
    Gen::ReturnCode getLastFill(LastFillEntry& lfe);
    const LastFills& getLastFills();
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

*/

} // namespace Ctrl

//----------------------------------------------------------------
