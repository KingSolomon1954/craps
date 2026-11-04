//----------------------------------------------------------------
//
// File: PlayerManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <filesystem>
#include <utility>
#include <vector>
#include <controller/PlayerFile.h>
#include <controller/PlayerManifest.h>
#include <controller/PlayerPrefs.h>
#include <craps/CrapsTypes.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

namespace Craps {
    class Player;  // fwd
}

namespace Ctrl {

class PlayerManager
{
public:
    using Players = std::vector<Craps::Player*>;

    /// @name Lifecycle
    /// @{
    PlayerManager();
   ~PlayerManager();
    /// @}

    /// @name Modifiers
    /// @{
    PlayerPrefs& userPrefs();
    PlayerPrefs* getPrefs(const Craps::PlayerId& playerId);
    /// @}

    /// @name Observers
    /// @{
    Gen::ReturnCode saveAll(Gen::ErrorPass& ep)       const;
    const PlayerPrefs& userPrefs()                    const;
    const PlayerManifest::PlayerList& getPlayerList() const;
    const Players& getPlayers()                       const;
    Craps::Player* getUserPlayer()                    const;
    Craps::Player* getPlayer(
        const Craps::PlayerId& playerId,
        Gen::ErrorPass& ep)                           const;
    /// @}

private:
    // Everything kept for one loaded player. The Player itself is owned
    // by players_, not by the Entry.
    struct Entry
    {
        Entry(Craps::Player* p, std::filesystem::path path)
            : player(p), file(std::move(path)) {}

        Craps::Player* player;
        PlayerFile     file;
        PlayerPrefs    prefs;
    };

    mutable PlayerManifest manifest_;
    Players players_;
    std::vector<std::unique_ptr<Entry>> entries_;

    Entry* findEntry(const Craps::Player* player) const;

    Craps::Player* loadPlayer(const std::string& fileName);
    void loadStartingPlayers();  // throws
    void addPlayersToTable();    // throws
    static std::filesystem::path formPlayerPath(const std::string& fileName);
};

/*-----------------------------------------------------------*//**

@class PlayerManager

@brief Managers players

Manages players.

Maintains a central index file (e.g., players/index.json) listing all
known player IDs and names for fast access/UI.

PlayerManager Responsibilities:

@li Maintain list of player IDs

@li Own each loaded player's file (PlayerFile) and controller-level
preferences (PlayerPrefs), and decide when they are read and written

@li Create new player

@li Choose active player

@par Player files

Craps::Player knows nothing about files. It only converts itself to and
from YAML. PlayerManager owns the rest:

@li loadPlayer() reads the player's file once, builds the Player from its
"Player" section and the PlayerPrefs from its "Prefs" section.
@li saveAll() takes a fresh snapshot of every Player and every
PlayerPrefs and writes each file. Call it once at quit, after the table
has shut the players down, since Player::shutdown() is what merges the
session into the stats.
@li userPrefs() is the preferences of whoever is the user player right
now. Switching users changes which PlayerPrefs this returns; there is
nothing to save or reload at the switch. Every loaded player's
preferences are written by saveAll() along with the rest of the player.
@li getPrefs() is the same for any player, by id.

*/

} // namespace Ctrl

//----------------------------------------------------------------
