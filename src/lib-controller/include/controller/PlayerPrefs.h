//----------------------------------------------------------------
//
// File: PlayerPrefs.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/LastFill.h>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class PlayerPrefs
{
public:
    /// @name Modifiers
    /// @{
    LastFill& lastFill();
    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    const LastFill& lastFill() const;
    YAML::Node toYAML() const;
    /// @}

private:
    LastFill lastFill_;
};

/*-----------------------------------------------------------*//**

@class PlayerPrefs

@brief One player's controller-level preferences

Things the controller remembers for a player that the Craps model knows
nothing about. Currently LastFill. AutoFill and QuickBet will be added
as members the same way.

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
@endcode

*/

} // namespace Ctrl

//----------------------------------------------------------------
