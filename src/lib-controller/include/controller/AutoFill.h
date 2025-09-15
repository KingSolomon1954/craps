//----------------------------------------------------------------
//
// File: AutoFill.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <gen/ReturnCode.h>

namespace Ctrl {

class AutoFill
{
public:
    struct AutoFillEntry
    {
        BetName    betName    = BetName::Invalid;
        size_t     pivot      = 0;
        bool       isOddsBet  = false;
        Gen::Money amount     = 0;
    };
    using Fills = std::vector<AutoFillEntry>;
    
    /// @name Lifecycle
    /// @{
    static AutoFill* instance();
    /// @}
    
    /// @name Modifiers
    /// @{
    void setAutoFill   (const AutoFillEntry& afe);
    void deleteAutoFill(const AutoFillEntry& afe);
    /// @}

    /// @name Observers
    /// @{
    Gen::ReturnCode getAutoFill(AutoFillEntry& afe);
    const Fills& getAutoFills();
    /// @}
    
private:
    Fills fills_;

    // Helper to find an entry by betName/pivot/isOddsBet
    Fills::iterator       findEntry(const AutoFillEntry& afe);
    Fills::const_iterator findEntry(const AutoFillEntry& afe) const;
};

/*-----------------------------------------------------------*//**

@class AutoFill

@brief Manages auto fill amounts for bets

The AutoFill class in controller has these responsibilities:

@li maintains a list of auto fill bets in std::vector

@li setAutoFill() - replaces amount if the entry already exists, zero is valid amount
@li getAutoFills() - returns the read-only vector of auto fill entries
@li deleteAutoFill() - shift entries if necessary, no empty slots

*/

} // namespace Ctrl

//----------------------------------------------------------------
