//----------------------------------------------------------------
//
// File: PlayerManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <unordered_map>
#include <gen/Uuid.h>
#include <controller/PlayerDescription.h>
#include <craps/Player.h>

namespace Craps {
    struct DecisionRecord;  // fwd
}
    
namespace Ctrl {

class PlayerManager
{
public:
    /// @name Lifecycle
    /// @{
    PlayerManager();
   ~PlayerManager() = default;
    bool loadPlayers();
    bool savePlayers();
    /// @}

    /// @name Modifiers
    /// @{
    using PlayerId = Gen::Uuid;
    using PlayerPtr = std::shared_ptr<class Craps::Player>;
    PlayerPtr createPlayer(const std::string& name);
    /// @}

    /// @name Observers
    /// @{
    PlayerPtr getPlayer(const Gen::Uuid& id) const;
    void disburseWin (const Craps::DecisionRecord& dr) const;
    void disburseLose(const Craps::DecisionRecord& dr) const;
    void disburseKeep(const Craps::DecisionRecord& dr) const;
    /// @}

    using PlayerDescriptions = std::vector<PlayerDescription>;
    static PlayerDescriptions loadPlayerChoices();
//    static Craps::Player loadPlayer(const Gen::Uuid& playerId);
    static Craps::Player loadPlayer(const PlayerId& playerId);

private:
    std::unordered_map<Gen::Uuid, std::shared_ptr<Craps::Player>> playersAll_;

    void diagBadPlayerId(const std::string& funcName,
                         const PlayerId& playerId) const;
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
