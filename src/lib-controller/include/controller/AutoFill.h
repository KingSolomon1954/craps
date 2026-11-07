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
#include <yaml-cpp/yaml.h>

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
    using AutoFills = std::vector<AutoFillEntry>;
    
    /// @name Lifecycle
    /// @{
    AutoFill() = default;
   ~AutoFill() = default;
    /// @}
    
    /// @name Modifiers
    /// @{
    void setAutoFill   (const AutoFillEntry& afe);
    void deleteAutoFill(const AutoFillEntry& afe);
    void clear();
    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    Gen::ReturnCode getAutoFill(AutoFillEntry& afe);
    const AutoFills& getAutoFills();
    YAML::Node toYAML() const;
    /// @}
    
private:
    AutoFills fills_;

    // Helper to find an entry by betName/pivot/isOddsBet
    AutoFills::iterator       findEntry(const AutoFillEntry& afe);
    AutoFills::const_iterator findEntry(const AutoFillEntry& afe) const;
};

/*-----------------------------------------------------------*//**

@class AutoFill

@brief Manages auto fill amounts for bets

The AutoFill class in controller has these responsibilities:

@li maintains a list of auto fill bets in std::vector

@li setAutoFill() - replaces amount if the entry already exists, zero is valid amount
@li getAutoFills() - returns the read-only vector of auto fill entries
@li deleteAutoFill() - shift entries if necessary, no empty slots
@li clear() - forgets all entries

@par Ownership and persistence

Not a singleton, and it publishes no events; it is plain data. Each
player has one, held in PlayerPrefs, which PlayerManager owns.
EventHandler changes the user's AutoFill and publishes UslAutoFillChanged.

toYAML() and fromYAML() convert between the entries and a YAML
sequence, in order (see BetEntryYaml.h). PlayerPrefs puts it under
"AutoFills" in the player file's "Prefs" section. fromYAML() replaces the
current contents and never throws; bad entries are logged and skipped.

*/

} // namespace Ctrl

//----------------------------------------------------------------
