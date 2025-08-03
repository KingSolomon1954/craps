//----------------------------------------------------------------
//
// File: PlayerManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <unordered_map>
#include <controller/PlayerDescription.h>
#include <craps/CrapsTypes.h>

namespace Craps {
    struct DecisionRecord;  
    class Player;           // fwd
}
    
namespace Ctrl {

class PlayerManager
{
public:
    /// @name Lifecycle
    /// @{
    PlayerManager();
   ~PlayerManager() = default;
    
    void loadStartingPlayers();
    bool loadPlayers();
    bool savePlayers();
    /// @}

    /// @name Modifiers
    /// @{
    Craps::PlayerPtr createPlayer(const std::string& name);
    /// @}

    /// @name Observers
    /// @{
    Craps::PlayerPtr getPlayer(const Craps::PlayerId& playerId) const;
    /// @}

    using PlayerDescriptions = std::vector<PlayerDescription>;
    
    static PlayerDescriptions loadPlayerChoices();
    static Craps::Player      loadPlayer(const Craps::PlayerId& playerId);

private:
    std::unordered_map<Craps::PlayerId, Craps::PlayerPtr> players_;
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
