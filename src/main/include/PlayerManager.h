//----------------------------------------------------------------
//
// File: PlayerManager.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <unordered_map>
#include <gen/Uuid.h>
#include "Player.h"

namespace App {

struct DecisionRecord;  // fwd

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
    using PlayerPtr = std::shared_ptr<class Player>;
    PlayerPtr createPlayer(const std::string& name);
    /// @}

    /// @name Observers
    /// @{
    PlayerPtr getPlayer(const Gen::Uuid& id) const;
    void disburseWin (const DecisionRecord& dr) const;
    void disburseLose(const DecisionRecord& dr) const;
    void disburseKeep(const DecisionRecord& dr) const;
    /// @}

private:
    std::unordered_map<Gen::Uuid, std::shared_ptr<Player>> playersAll_;

    void diagBadPlayerId(const std::string& funcName,
                         const Gen::Uuid& playerId) const;
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

} // namespace App

//----------------------------------------------------------------
