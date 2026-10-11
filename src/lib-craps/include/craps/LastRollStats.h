//----------------------------------------------------------------
//
// File: LastRollStats.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/MoneyUtils.h>

namespace Craps {

struct LastRollStats
{
    unsigned rollCount        = 0;
    Gen::Money amountOnTable  = 0;
    Gen::Money amountWin      = 0;
    Gen::Money amountLose     = 0;
    Gen::Money numBetsOnTable = 0;
    Gen::Money numBetsWin     = 0;
    Gen::Money numBetsLose    = 0;
    
    void reset()
    {
        rollCount      = 0;
        amountOnTable  = 0;
        amountWin      = 0;
        amountLose     = 0;
        numBetsOnTable = 0;
        numBetsWin     = 0;
        numBetsLose    = 0;
    }
    void prep(Gen::Money amountTable, unsigned numBets)
    {
        reset();
        amountOnTable  = amountTable;
        numBetsOnTable = numBets;
    }
};

/*-----------------------------------------------------------*//**

@struct LastRollStats

@brief Statistics regarding the previous roll

*/

} // namespace Craps

//----------------------------------------------------------------
