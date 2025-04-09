//----------------------------------------------------------------
//
// File: Bet.h
//
//----------------------------------------------------------------

#pragma once

#include "Globals.h"
#include "EnumBetName.h"

namespace App {

class Bet
{
public:
    /// @name Lifecycle
    /// @{
    Bet();
   ~Bet() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

    BetName betName = BetName::Invalid;
    unsigned int pivot = 0;
    Money contractBet = 0;
    Money oddsBet = 0;
    // PlayerId playerId;
    // TableId tabledId;
    // Time creationTime;
    // Time decisionTime;
    unsigned int distance = 0;
    Money win = 0;
    Money lose = 0;
    
private:
    
};

/*-----------------------------------------------------------*//**

@class Bet

@brief A bet on the craps table.

Store info that represents a craps bet.

*/

} // namespace App

//----------------------------------------------------------------
