//----------------------------------------------------------------
//
// File: DecisionRecord.h
//
//----------------------------------------------------------------

#pragma once

#include <ostream>
#include <craps/Globals.h>
#include <gen/Uuid.h>

namespace Craps {

struct DecisionRecord
{
    unsigned betId = 0;
    bool decision = false;
    bool pivotAssigned = false;
    Money win = 0;
    Money lose = 0;
    Money returnToPlayer = 0;
    Money commission = 0;
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
};

/*-----------------------------------------------------------*//**

@struct DecisionRecord

@brief Results of a bet after dice throw

Results of a bet after dice throw.

*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::DecisionRecord& dr);

//----------------------------------------------------------------
