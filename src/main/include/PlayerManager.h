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
    void disburseWin(const DecisionRecord& dr) const;
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

*/

} // namespace App

//----------------------------------------------------------------
