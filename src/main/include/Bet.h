//----------------------------------------------------------------
//
// File: Bet.h
//
//----------------------------------------------------------------

#pragma once

#include "Globals.h"

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

    // EnumBetName betName;
    unsigned int pivot = 0;
    Money contractBet = 0;
    Money oddsBet = 0;
    // PlayerId playerId;
    // TableId tabledId;
    // Time creationTime;
    // Time decisionTime;
    unsigned int distance;
    Money win = 0;
    Money lose = 0;
    
private:
    struct OddsEntry
    {
        unsigned int numerator;
        unsigned int denominator;
    };

    OddsEntry oddsPass[13] =
    {
        {0,0},    // 0
        {0,0},    // 1
        {0,0},    // 2
        {0,0},    // 3
        {2,1},    // 4
        {3,2},    // 5
        {6,5},    // 6
        {0,0},    // 7
        {6,5},    // 8
        {3,2},    // 9
        {2,1},    // 10
        {0,0},    // 11
        {0,0}     // 12
    };
    
    OddsEntry oddsDont[13] =
    {
        {0,0},    // 0
        {0,0},    // 1
        {0,0},    // 2
        {0,0},    // 3
        {1,2},    // 4
        {2,3},    // 5
        {5,6},    // 6
        {0,0},    // 7
        {5,6},    // 8
        {2,3},    // 9
        {1,2},    // 10
        {0,0},    // 11
        {0,0}     // 12
    };
    
    OddsEntry oddsPlace[13] =
    {
        {0,0},    // 0
        {0,0},    // 1
        {0,0},    // 2
        {0,0},    // 3
        {9,5},    // 4
        {7,5},    // 5
        {7,6},    // 6
        {0,0},    // 7
        {7,6},    // 8
        {7,5},    // 9
        {9,5},    // 10
        {0,0},    // 11
        {0,0}     // 12
    };
    
    OddsEntry oddsHardway[13] =
    {
        {0,0},    // 0
        {0,0},    // 1
        {0,0},    // 2
        {0,0},    // 3
        {7,1},    // 4
        {0,0},    // 5
        {9,1},    // 6
        {0,0},    // 7
        {9,1},    // 8
        {0,0},    // 9
        {7,1},    // 10
        {0,0},    // 11
        {0,0}     // 12
    };
    
};

/*-----------------------------------------------------------*//**

@class Bet

@brief A bet on the craps table.

Store info that represents a craps bet.

*/

} // namespace App

//----------------------------------------------------------------
