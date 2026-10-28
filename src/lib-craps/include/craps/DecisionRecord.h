//----------------------------------------------------------------
//
// File: DecisionRecord.h
//
//----------------------------------------------------------------

#pragma once

#include <ostream>
#include <gen/MoneyUtils.h>
#include <gen/Uuid.h>

namespace Craps {

class CrapsBet;  // fwd

struct DecisionRecord
{
    CrapsBet* pBet            = nullptr;
    bool decision             = false;
    bool pivotAssigned        = false;
    Gen::Money win            = 0;
    Gen::Money lose           = 0;
    Gen::Money returnToPlayer = 0;
    Gen::Money commission     = 0;
    // An outcome may be resolved while the wager remains active (e.g. a winning Place bet).
    bool removeBet            = true;

    bool operator==(const DecisionRecord& other) const
    {
        return pBet           == other.pBet           &&
               decision       == other.decision       &&
               pivotAssigned  == other.pivotAssigned  &&
               win            == other.win            &&
               lose           == other.lose           &&
               returnToPlayer == other.returnToPlayer &&
               commission     == other.commission     &&
               removeBet      == other.removeBet;
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
        removeBet      = true;
    }
};

/*-----------------------------------------------------------*//**

@struct DecisionRecord

@brief Results of a bet after a dice throw

The structure is used to convey the results of a bet after a dice throw.

The DecisionRecord contains the following fields to convey handling by
the caller:

@var DecisionRecord::pBet
    The bet associated with this result

@var DecisionRecord::decision
    Indicates that a decision has been reached for this bet.

    When true, this bet has a resolved outcome on this roll. One or more
    of the win, lose, or returnToPlayer fields may be non-zero. This does
    not by itself mean the wager has ended; see removeBet.

    When false, this bet has not reached a decision and should remain on
    on the Craps table. The win, lose, retrunToPlayer fields will all be
    zero. The distance field (in the CrapsBet) is incremented by one. On
    come out rolls (i.e, the passed in point is zero) then the CrapsBet
    pivot field, if zero, will be set to the dice value for
    Pass/Come/DontPass/DontCome bets.

@var DecisionRecord::removeBet
    Indicates whether the wager should be removed from the player and table
    after this outcome is settled. A winning Place, Buy, Lay, or Hardway bet
    can resolve a win while remaining active.

@var DecisionRecord::pivotAssigned
    Indicates whether a pivot for this bet has been freshly
    established. For example, a Come bet has now been moved to the "6".

    When true, the pivot has been assigned. Caller uses this
    flag to move a come bet on the table to its box.

    When evaluating PassLine bets, if somehow a pivot is zero and the
    point has already been established, then the pivot is silently
    assigned to the already established point, as if the player made a
    PassLine bet after point was established.

@var DecisionRecord::win
    If non-zero, then this bet has won the given amount.

    The calculation includes odds winnings and is net of any commission.
    If removeBet is true, the caller returns the original contract and odds
    stakes along with this profit. If removeBet is false, only this profit is
    credited because the original stake remains active on the table.

@var DecisionRecord::lose
    If non-zero, then this bet has lost the given amount.

    The amount is the contractBet plus odds bet, if any. The caller
    implementation should add this amount to the table's banking system
    and subtract this amount from the player's wallet if not already
    done so.

@var DecisionRecord::returnToPlayer
    If non-zero, the given amount should be returned to the player.

    This is used when odds are off during a come-out roll. It can accompany
    either a win or a loss. When a winning settlement also returns the
    original odds stake, the caller must avoid returning that stake twice.

@var DecisionRecord::commission
    Lay and Buy bets require a 5% commission if they win.

    This amount has been subtracted from the win amount.  The table's
    banking system should deposit this amount if non-zero.
*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::DecisionRecord& dr);

//----------------------------------------------------------------
