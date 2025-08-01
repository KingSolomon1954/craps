//----------------------------------------------------------------
//
// File: DecisionRecord.h
//
//----------------------------------------------------------------

#pragma once

#include <ostream>
#include <gen/MoneyUtil.h>
#include <gen/Uuid.h>

namespace Craps {

struct DecisionRecord
{
    CrapsBet* pBet            = nullptr;
    bool decision             = false;
    bool pivotAssigned        = false;
    Gen::Money win            = 0;
    Gen::Money lose           = 0;
    Gen::Money returnToPlayer = 0;
    Gen::Money commission     = 0;

    bool operator==(const DecisionRecord& other) const
    {
        return pBet           == other.pBet           &&
               decision       == other.decision       &&
               pivotAssigned  == other.pivotAssigned  &&
               win            == other.win            &&
               lose           == other.lose           &&
               returnToPlayer == other.returnToPlayer &&
               commission     == other.commission;
    }
    void reset()
    {
        pBet           = nullptr;
        decision       = false;
        pivotAssigned  = false;
        win            = 0;
        lose           = 0;
        returnToPlayer = 0;
        commission     = 0;
    }
};

/*-----------------------------------------------------------*//**

@struct DecisionRecord

@brief Results of a bet after a dice throw

The structure is used to convey the results of a bet after a dice throw.

The DecisionRecord contains the following fields to convey handling by
the caller:

@var pBet - the bet associated with this result

@var decision -indicates that a decision has been reached for this bet.

    When true, this bet has reached a decision, and one or more of the
    win, lose, returnToPlayer fields in this record will have non-zero
    values. It is the caller's responsibility to remove the bet from
    the craps table.

    When false, this bet has not reached a decision and should remain on
    on the Craps table. The win, lose, retrunToPlayer fields will all be
    zero. The distance field (in the CrapsBet) is incremented by one. On
    come out rolls (i.e, the passed in point is zero) then the CrapsBet
    pivot field, if zero, will be set to the dice value for
    Pass/Come/DontPass/DontCome bets.

@var pivotAssigned - indicates whether a pivot for this bet has been freshly
    established. For example, a Come bet has now been moved to the "6".

    When true, the pivot has been assigned. Caller uses this
    flag to move a come bet on the table to its box.

    When evaluating PassLine bets, if somehow a pivot is zero and the
    point has already been established, then the pivot is silently
    assigned to the already established point, as if the player made a
    PassLine bet after point was established.

@var win - if non-zero, then this bet has won the given amount.

    The calculation includes odds winnings. The caller implementation
    should take this win amount and add it with the original
    contractAmount and oddsAmount to give back to the player.

@var lose - if non-zero, then this bet has lost the given amount.

    The amount is the contractBet plus odds bet, if any. The caller
    implementation should add this amount to the table's banking system
    and subtract this amount from the player's wallet if not already
    done so.

@var returnToPlayer - if non-zero then the given amount should be returned
    to the player.

    Note this is not a win or lose. This happens when a point-based bet
    wins/loses during a come out roll and setOffComeOutRoll() is true.

@var commission - Lay and Buy bets require a 5% commission if they win.

    This amount has been subtracted from the win amount.  The table's
    banking system should deposit this amount if non-zero.
*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::DecisionRecord& dr);

//----------------------------------------------------------------
