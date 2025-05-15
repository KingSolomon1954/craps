//----------------------------------------------------------------
//
// File: PlayerManager.h
//
//----------------------------------------------------------------

#pragma once

namespace App {

struct DecisionRecord;  // fwd
    
class PlayerManager
{
public:
    /// @name Lifecycle
    /// @{
    PlayerManager();
   ~PlayerManager() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    void disburseWin (const DecisionRecord& dr) const;
    void disburseLose(const DecisionRecord& dr) const;
    void disburseKeep(const DecisionRecord& dr) const;
    /// @}

private:
    unsigned d1_ = 6; 
    unsigned d2_ = 6; 
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
