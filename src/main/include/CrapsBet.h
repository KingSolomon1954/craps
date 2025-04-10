//----------------------------------------------------------------
//
// File: CrapsBet.h
//
//----------------------------------------------------------------

#pragma once

#include "Globals.h"
#include "EnumBetName.h"

namespace App {

class CrapsBet
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBet();
   ~CrapsBet() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

    BetName betName = BetName::Invalid;
    unsigned pivot = 0;
    Money contractBet = 0;
    Money oddsBet = 0;
    bool oddsOffComeOutRoll = true;
    // PlayerId playerId;
    // TableId tabledId;
    // Time whenCreated;
    // Time whenDecided;
    unsigned distance = 0;  // num rolls until decision
    bool decision = false;
    Money win = 0;
    Money lose = 0;
    Money returnToPlayer = 0;
    
private:
    
};

/*-----------------------------------------------------------*//**

@class CrapsBet

@brief A bet on the craps table.

Store info that represents a craps bet.

*/

} // namespace App

//----------------------------------------------------------------
