//----------------------------------------------------------------
//
// File: CrapsBet.cpp
//
//----------------------------------------------------------------

#include "CrapsBet.h"

#include <stdexcept> // for std::invalid_argument
#include "Dice.h"
#include "OddsTables.h"

using namespace App;

unsigned CrapsBet::idCounter_ = 0;

/*-----------------------------------------------------------*//**

Constructor - creates a CrapsBet

Creates a Craps bet. The bet is given a unique ID and
time of creation.

@param[in] name
    The name/type of the bet. Throws if this is not valid name.

@param[in] contractAmount
    The amount of the bet. Throws if this is zero.

@param[in] pivot
    The number this bet is focused on. For example a Place bet must set
    the pivot set to 4,5,6,8,9,10 otherwise an exception is thrown. For
    Pass/Come/DontCome/DontPass bets, the caller sets the pivot to zero
    to indicate a point number will bet set later, otherwise the pivot
    must be 4,5,6,8,9,10. For Field, AnyCraps, C&E bets, the pivot is
    unused and set to 0.
    
*/
CrapsBet::CrapsBet(BetName name, Money contractAmount, unsigned pivot)
    : betId_(++idCounter_)
    , betName_(name)
    , pivot_(pivot)
    , contractAmount_(contractAmount)
    , oddsAmount_(0)
    , oddsOffComeOutRoll_(true)
    , distance_(0)
    , whenCreated_(std::chrono::system_clock::now())
{
    validArgsCtor(); // Throws if bad args
}

/*-----------------------------------------------------------*//**

Determine if the bet is suitable for processing.

For example Place bets must have the pivot set to 
4,5,6,8,9 or 10 and the contractAmount must be non-zero.

*/
void
CrapsBet::validArgsCtor()
{
    if (betName_ == BetName::Invalid)
    {
        throw std::invalid_argument("CrapsBet()::ctor Bad \"betName\": cannot be \"BetName::Invalid\".");
    }
    if (contractAmount_ == 0)
    {
        throw std::invalid_argument("CrapsBet()::ctor Bad \"contractAmount\": must be > 0.");
    }
    if ((betName_ == BetName::PassLine) ||
        (betName_ == BetName::Come)     ||     
        (betName_ == BetName::DontPass) ||
        (betName_ == BetName::DontCome))
    {
        if ((pivot_ != 0) && (pivot_ != 4) && (pivot_ != 5) &&
            (pivot_ != 6) && (pivot_ != 8) && (pivot_ != 9) &&
            (pivot_ != 10))
        {
            throw std::invalid_argument(
                "CrapsBet()::ctor Bad \"pivot\": for "
                "PassLine|Come|DontPass|Dontcome bet, pivot must "
                "be 0,4,5,6,8,9 or 10.");
        }
    }
    if ((betName_ == BetName::Place) ||
        (betName_ == BetName::Lay)   ||
        (betName_ == BetName::Buy))
    {
        if ((pivot_ != 4) && (pivot_ != 5) && (pivot_ != 6)  &&
            (pivot_ != 8) && (pivot_ != 9) && (pivot_ != 10))
        {
            throw std::invalid_argument(
                "CrapsBet()::ctor Bad \"pivot\": for a Place, Buy, or Lay "
                "bet, pivot must be one of 4,5,6,8,9,10");
        }
    }
    if ((betName_ == BetName::Field)    ||
        (betName_ == BetName::AnyCraps) ||
        (betName_ == BetName::CandE))
    {
        pivot_ = 0;  // Quietly force it to 0.
    }
}

/*-----------------------------------------------------------*//**

Evaluates a CrapsBet for win/lose

Using the suppplied point and the current dice roll, evaluate()
determines whether this bet has won, lost or not yet reached a
decision. The results of the evaluation are returned in a DecisionRecord.

The DecisionRecord contains the following fields to
convey handling by the caller:

decision -indicates that a decision has been reached for this bet.

    When true, this bet has reached a decision, and one or more of the
    win, lose, returnToPlayer fields (below) will have non-zero
    values. It is the caller's responsibility to remove the bet from
    the craps table.

    When false, this bet has not reached a decision and should remain on
    on the Craps table. The win, lose, retrunToPlayer fields will
    all be zero. The distance field is incremented by one.  On come out
    rolls the pivot field will be set for Pass/Come/DontPass/DontCome
    bets.

win - if non-zero, then this bet has won the given amount of money. The
calculation includes odds winnings. The caller implementation should
take the win amount, add it with the original contractBet and oddsBet
and give that amount back to the player.

lose - if non-zero, then this bet has lost the given amount of
money. The amount is just the contractBet plus odds bet, if any. The
caller implementation should add this much money to the table's banking
system and subtract this amount from the player's wallet if not already
done so.

returnToPlayer - if non-zero then the given amount of money should be
returned to the player. Note this is not a win. This can happen when a
box bet wins/loses during a come out roll and the optional setting of
oddsOffComeOutRoll is true.

*/
bool
CrapsBet::evaluate(unsigned point, const Dice& dice)
{
    DecisionRecord dr(betId_); (void) dr;
    if (!validArgsEval()) return false;
                         
    distance_++;
    
    switch (betName_)
    {
    case BetName::PassLine: evalPassLine(point, dice); break;
    default: return true;

    }
    return true;
}

//----------------------------------------------------------------

bool
CrapsBet::evalPassLine(unsigned point, const Dice& dice)
{
    (void) point;
    (void) dice;
    whenDecided_ = std::chrono::system_clock::now();
    return true;
}

//----------------------------------------------------------------

bool
CrapsBet::validArgsEval()
{
    return true;    
}

/*-----------------------------------------------------------*//**

Returns the bet ID.

@return
    The unique bet ID associated with this bet.

*/
unsigned
CrapsBet::betId() const
{
    return betId_;
}

/*-----------------------------------------------------------*//**

Returns the bet name/type.

@return
    The bet name/type.

*/
BetName
CrapsBet::betName() const
{
    return betName_;
}

/*-----------------------------------------------------------*//**

Returns the pivot.

The pivot is the focus number for this bet. For example a Place bet will
have a pivot of 4,5,6,8,9, or 10. For initial Pass/Come/DontCome/DontPass bets,
the pivot is zero indicating a point number is not yet established.

@return
    The pivot.

*/
unsigned
CrapsBet::pivot() const
{
    return pivot_;
}

/*-----------------------------------------------------------*//**

Returns the contract amount of the bet.

Contract amount does not include the odds amount.

@return
    The contract amount.

*/
unsigned
CrapsBet::contractAmount() const
{
    return contractAmount_;
}

/*-----------------------------------------------------------*//**

Returns the odds amount of the bet.

Odd amount does not include the contract amount.

@return
    The odds amount.

*/
unsigned
CrapsBet::oddsAmount() const
{
    return oddsAmount_;
}

/*-----------------------------------------------------------*//**

Returns whether odds are off on the come out roll.

@return
    true if odds are off on the come out roll, false otherwise.

*/
bool
CrapsBet::oddsOffComeOutRoll() const
{
    return oddsOffComeOutRoll_;
}

/*-----------------------------------------------------------*//**

Returns the number of dice rolls to reach a decision.

If a decision is not yet reached it is the number of
dice rolls so far.

@return
    number of dice rolls so far to reach a decision.

*/
unsigned
CrapsBet::distance() const
{
    return distance_;
}

/*-----------------------------------------------------------*//**

Returns the time of the bet creation.

@return
    time of bet creation.

*/
std::chrono::time_point<std::chrono::system_clock>
CrapsBet::whenCreated() const
{
    return whenCreated_;
}

/*-----------------------------------------------------------*//**

Returns the time the bet reached a decision.

@return
    time of bet decision.

*/
std::chrono::time_point<std::chrono::system_clock>
CrapsBet::whenDecided() const
{
    return whenDecided_;
}

//----------------------------------------------------------------


