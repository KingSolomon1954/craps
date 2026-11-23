//----------------------------------------------------------------
//
// File: PlayerPrefs.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/AutoOdds.h>
#include <controller/LastFill.h>
#include <controller/QuickBets.h>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class PlayerPrefs
{
public:
    /// @name Modifiers
    /// @{
    LastFill& lastFill();
    QuickBets& quickBets();
    AutoOdds& autoOdds();
    void fromYAML(const YAML::Node& node);
    /// @}

    /// @name Observers
    /// @{
    const LastFill& lastFill() const;
    const QuickBets& quickBets() const;
    const AutoOdds& autoOdds() const;
    YAML::Node toYAML() const;
    /// @}

private:
    LastFill lastFill_;
    QuickBets quickBets_;
    AutoOdds autoOdds_;
};

/*-----------------------------------------------------------*//**

@class PlayerPrefs

@brief One player's controller-level preferences

Things the controller remembers for a player that the Craps model knows
nothing about: LastFill, QuickBets and AutoOdds, which are plain data. To
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
  QuickBets: [...]    # see QuickBets.h
  AutoOdds: {...}     # see AutoOdds.h
@endcode

Each list is read on its own, so a bad or missing list only empties
that list. Keys it does not know, such as the AutoFills of older player files,
are ignored and are not written back.

*/

} // namespace Ctrl

//----------------------------------------------------------------
