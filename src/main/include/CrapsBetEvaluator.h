//----------------------------------------------------------------
//
// File: CrapsBetEvaluator.h
//
//----------------------------------------------------------------

#pragma once

#include "Globals.h"

namespace App {

class CrapsBet;  // fwd
class Dice;      // fwd
    
class CrapsBetEvaluator
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBetEvaluator();
   ~CrapsBetEvaluator() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}


   static bool evaluate(CrapsBet& bet, unsigned point, const Dice& dice); // , const Dice& dice);
//  static ReturnCode evaluate(CrapsBet& bet, unsigned point, const Dice& dice, ErrorPass& ep);

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

    // Used for calculating payouts in Field bets
    OddsEntry oddsField[13] =
    {
        {0,0},    // 0
        {0,0},    // 1
        {2,1},    // 2
        {1,1},    // 3
        {1,1},    // 4
        {0,0},    // 5
        {0,0},    // 6
        {0,0},    // 7
        {0,0},    // 8
        {1,1},    // 9
        {1,1},    // 10
        {0,0},    // 11
        {3,1}     // 12
    };

    enum Decision
    {
        Win,
        Lose,
        Keep
    };

};

/*-----------------------------------------------------------*//**

@class CrapsBetEvaluator

@brief Evaluates a CrapsBet for win/lose.

The CBE takes in a bet instance, the point, and the current dice roll
and evaluates whether the bet has won, lost or not yet reached a
decision. The results of the evaluation are returned in the very same
bet instance with several fields filled in.

*/

} // namespace App

//----------------------------------------------------------------
