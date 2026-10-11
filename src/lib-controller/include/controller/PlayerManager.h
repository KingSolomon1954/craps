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

*/

} // namespace Ctrl

//----------------------------------------------------------------
