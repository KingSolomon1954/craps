//----------------------------------------------------------------
//
// File: PlayerManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <filesystem>
#include <controller/PlayerManifest.h>
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
    void saveBetPrefs    (Craps::Player& player) const;
    void loadBetPrefs    (const Craps::Player& player) const;
    void saveUserBetPrefs() const;
    void loadUserBetPrefs() const;
    /// @}

    /// @name Observers
    /// @{
    const PlayerManifest::PlayerList& getPlayerList() const;
    const Players& getPlayers()                       const;
    Craps::Player* getUserPlayer()                    const;
    Craps::Player* getPlayer(
        const Craps::PlayerId& playerId,
        Gen::ErrorPass& ep)                           const;
    /// @}

private:
    mutable PlayerManifest manifest_;
    Players players_;

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

@li Load/save players from disk

@li Create new player

@li Choose active player

@li Use Player::serialize() and Player::deserialize() for actual data

@par Bet preferences

The user's bet preferences (currently LastFill) live in the controller's
singletons while the game runs, but belong to a Player's file. The
Player keeps them as an opaque "BetPreferences" YAML section.

@li saveBetPrefs()  - copies the singletons into a Player (in memory)
@li loadBetPrefs()  - fills the singletons from a Player
@li saveUserBetPrefs(), loadUserBetPrefs() - same, for the user's player

Startup calls loadUserBetPrefs() once the players are loaded. Quit calls
saveUserBetPrefs() just before the table shuts down, since that is what
makes each Player write its file. When the user switches players at
runtime: saveBetPrefs(oldPlayer), change the user, then
loadBetPrefs(newPlayer). The old player's file gets written along with
all the others at quit, like the rest of that player's data.

*/

} // namespace Ctrl

//----------------------------------------------------------------
