//----------------------------------------------------------------
//
// File: DecisionRecord.h
//
//----------------------------------------------------------------

#pragma once

#include <ostream>
#include <controller/Globals.h>
#include <gen/MoneyUtil.h>
#include <gen/Uuid.h>

namespace Craps {

struct DecisionRecord
{
    unsigned betId = 0;
    bool decision = false;
    bool pivotAssigned = false;
    Gen::Money win = 0;
    Gen::Money lose = 0;
    Gen::Money returnToPlayer = 0;
    Gen::Money commission = 0;
    Gen::Uuid playerId;

    bool operator==(const DecisionRecord& other) const
    {
        return betId == other.betId &&
               decision == other.decision &&
               pivotAssigned == other.pivotAssigned &&
               win == other.win &&
               lose == other.lose &&
               returnToPlayer == other.returnToPlayer &&
               commission == other.commission &&
               playerId == other.playerId;
    }
    void reset()
    {
        betId = 0;
        decision = false;
        pivotAssigned = false;
        win = 0;
        lose = 0;
        returnToPlayer = 0;
        commission = 0;
        playerId = "";
    }
};

/*-----------------------------------------------------------*//**

@struct DecisionRecord

@brief Results of a bet after dice throw

Results of a bet after dice throw.

*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::DecisionRecord& dr);

//----------------------------------------------------------------
