//----------------------------------------------------------------
//
// File: PlayerPrefs.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/AutoFill.h>
#include <controller/LastFill.h>
#include <controller/QuickBet.h>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class PlayerPrefs
{
public:
    /// @name Modifiers
    /// @{
    LastFill& lastFill();
    AutoFill& autoFill();
    QuickBet& quickBet();
    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    const LastFill& lastFill() const;
    const AutoFill& autoFill() const;
    const QuickBet& quickBet() const;
    YAML::Node toYAML() const;
    /// @}

private:
    LastFill lastFill_;
    AutoFill autoFill_;
    QuickBet quickBet_;
};

/*-----------------------------------------------------------*//**

@class PlayerPrefs

@brief One player's controller-level preferences

Things the controller remembers for a player that the Craps model knows
nothing about: LastFill, AutoFill and QuickBet. Each is plain data. To
add another, add a member here and a key in toYAML() and fromYAML().

PlayerManager owns one PlayerPrefs per player. Observers reach the
user's through PlayerManager::userPrefs(). Nothing here is a singleton,
so changing who the user is just changes which PlayerPrefs is returned.

Persistence: toYAML() produces the player file's "Prefs" section and
fromYAML() restores from it. fromYAML() is forgiving; a missing or
malformed section yields empty preferences and never throws.

@code
Prefs:
  LastFills:
    - betName: PassLine
      pivot: 0
      isOddsBet: false
      amount: 50
  AutoFills: [...]    # same shape as LastFills
  QuickBets: [...]    # same shape; order matters
@endcode

Each of the three lists is read on its own, so a bad or missing list
only empties that list.

*/

} // namespace Ctrl

//----------------------------------------------------------------
