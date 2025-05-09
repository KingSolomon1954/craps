//----------------------------------------------------------------
//
// File: DecisionRecord.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <ostream>
#include "gen/Uuid.h"
#include "Globals.h"

namespace App {

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

} // namespace App

std::ostream& operator<< (std::ostream& out, const App::DecisionRecord& dr);

//----------------------------------------------------------------
