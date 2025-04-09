//----------------------------------------------------------------
//
// File: CrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include "Globals.h"
#include "Bet.h"

namespace App {

class CrapsTable
{
public:
    /// @name Lifecycle
    /// @{
    CrapsTable();
   ~CrapsTable() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    struct OddsEntry
    {
        unsigned int numerator;
        unsigned int denominator;
    };

    // Used for calculating payouts in Pass, Come, Buy bets
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
    
    // Used for calculating payouts in DontPass, DontCome, Lay bets
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
    
    // Used for calculating payouts in Place bets
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
    
    // Used for calculating payouts in Hardway bets
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

    enum Decision
    {
        Win,
        Lose,
        Keep
    };

    // void winLose(Bet& bet; unsigned int point, const Dice& dice);
};

/*-----------------------------------------------------------*//**

@class Bet

@brief A bet on the craps table.

Store info that represents a craps bet.

*/

} // namespace App

//----------------------------------------------------------------
