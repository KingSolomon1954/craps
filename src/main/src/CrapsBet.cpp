//----------------------------------------------------------------
//
// File: CrapsBet.cpp
//
//----------------------------------------------------------------

#include "CrapsBet.h"

#include <stdexcept> // for std::invalid_argument
#include <iostream>
#include "gen/ErrorPass.h"
#include "gen/StringUtils.h"
#include "Dice.h"
// #include "OddsTables.h"

using namespace App;

unsigned CrapsBet::idCounter_ = 0;

/*-----------------------------------------------------------*//**

Constructor - creates a CrapsBet

Creates a Craps bet. The bet is given a unique ID and time of creation.
The CrapsBet class does not store or use betIds for any purpose. betId
is meant for upper level designs that track many bets.

Arguments are checked for validity and if found to be bad then
std::invalid_argument is thrown.

Note that no check is made at construction time whether the bet is
allowed to be placed on a Craps table. CrapsBet is unaware of the state
of any Craps table. This decouples the act of creating a bet from
placing a bet on a CrapsTable, allowing the application better freedom
of design.

@param[in] name
    The name/type of the bet. Throws if this is not valid name.

@param[in] contractAmount
    The amount of the bet. Throws if this is zero.

@param[in] pivot
    The number this bet is focused on. For example a Place bet must set
    the pivot to 4,5,6,8,9 or 10 otherwise a std::invalid_argument
    exception is thrown.

    For Field, AnyCraps and C&E bets, the pivot is unused and internally
    set to 0.

    For PassLine/Come/DontPass/DontCome bets, the caller sets the pivot
    to zero. Zero indicates the pivot number needs to be set later.
    These point-based bets get their pivot number assigned during
    CrapsBet::evaluation(),

    As a special case only for PassLine bets, the caller may also
    explicitly set the pivot to one of 4,5,6,8,9 or10. This supports the
    behavior of putting down a PassLine bet after the point is already
    established.

    Note that CrapsBet constructor knows nothing about the state of the
    Craps table. It is the responsibility of the caller to ensure the
    bet is permissable and placed on a Craps table at the right time.

    For PassLine bets, if somehow a pivot is zero and the bet is being
    evaluated after a point has already been established, then the pivot
    is silently assigned to the already established point, as if the player
    made a PassLine bet after point was established.

    Come/DontPass/DontCome bets are rejected if pivot is non-zero, and an
    exception is thrown.

    For Pass/Come/DontCome/DontPass bets, the caller sets the pivot to
    zero to indicate the pivot number needs to be set later, or to
    4,5,6,8,9,10, otherwise a std::invalid_argument exception is thrown.
    Later, when CrapsBet::evaluate() is called, the pivot is set 
    to the supplied point.
*/
CrapsBet::CrapsBet(BetName name, Money contractAmount, unsigned pivot)
    : betId_(++idCounter_)
    , betName_(name)
    , pivot_(pivot)
    , contractAmount_(contractAmount)
    , oddsAmount_(0)
    , oddsBetOffComeOutRoll_(true)
    , placeBetOffComeOutRoll_(true)
    , hardwayBetOff_(false)
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
    if (betName_ == BetName::PassLine ||
        betName_ == BetName::Come     ||     
        betName_ == BetName::DontPass ||
        betName_ == BetName::DontCome)
    {
        if (!pointNums_.contains(pivot_) && pivot_ != 0)
        {
            throw std::invalid_argument(
                "CrapsBet()::ctor Bad \"pivot\": for "
                "PassLine|Come|DontPass|Dontcome bet, pivot must "
                "be 0,4,5,6,8,9 or 10.");
        }
    }
    if (betName_ == BetName::Place ||
        betName_ == BetName::Lay   ||
        betName_ == BetName::Buy)
    {
        if (!pointNums_.contains(pivot_))
        {
            throw std::invalid_argument(
                "CrapsBet()::ctor Bad \"pivot\": for a Place, Buy, or Lay "
                "bet, pivot must be one of 4,5,6,8,9,10");
        }
    }
    if (betName_ == BetName::Hardway)
    {
        if (!hardwayNums_.contains(pivot_))
        {
            throw std::invalid_argument(
                "CrapsBet()::ctor Bad \"pivot\": for Hardway "
                "bet, pivot must be one of 4,6,8,10");
        }
    }
    
    if (betName_ == BetName::Field    ||
        betName_ == BetName::AnyCraps ||
        betName_ == BetName::CandE)
    {
        pivot_ = 0;  // Quietly force it to 0.
    }
}

/*-----------------------------------------------------------*//**

Set, change, or remove the amount for an odds bet.

It is only permissable to set an odds amount if the following
conditions are true:

* the bet is a PassLine, DontPass, Come, DontCome bet

* the bet has already been assigned its pivot (point) (i.e, pivot is
non-zero)

* subject to checking house limits for max odds

* minimum bet is given by house rules or the actual dice odds whichever
* is higher

TODO: Also the house limits the amount of the odds bet to 10x the
amount of the contract bet.

@param [in] amount
    The amount to set it to. Clobbers any previous setting.

@returns
    Success if the bet was accepted, otherwise Fail and ep has
    the reason.
*/

Gen::ReturnCode
CrapsBet::setOddsAmount(Money amount, Gen::ErrorPass& ep)
{
    if (betName_ != BetName::PassLine && betName_ != BetName::DontPass &&
        betName_ != BetName::Come     && betName_ != BetName::DontCome)
    {
        std::string s("CrapsBet::setOddsAmount(): "
            "an odds bet is only allowed for these bets: "
            "PassLine|Come|DontPass|DontCome. Current bet is betId:");
        s += std::to_string(betId_) + " betName:" +
            EnumBetName::toString(betName_) + ". ";
        ep.diag = s;
        return Gen::ReturnCode::Fail;
    }

    if (pivot_ == 0)
    {
        std::string s("CrapsBet::setOddsAmount(): odds bet is not "
            "allowed until after a point is established for this bet. ");
        s += "betId:" + std::to_string(betId_) + " betName:" +
            EnumBetName::toString(betName_) + " pivot:" +
            std::to_string(pivot_) + ".";
        ep.diag = s;
        return Gen::ReturnCode::Fail;
    }

    if (betName_ == BetName::PassLine || betName_ == BetName::Come)
    {
        if (amount < OddsTables::oddsPass[pivot_].denominator)
        {
            std::string s("CrapsBet::setOddsAmount(): odds bet is too small. "
                "Minimum odds bet for a " + EnumBetName::toString(betName_) +
                "(" + std::to_string(pivot_) + ") is " +
                std::to_string(OddsTables::oddsPass[pivot_].denominator) + ".");
            ep.diag = s;
            return Gen::ReturnCode::Fail;
        }
    }
    if (betName_ == BetName::DontPass || betName_ == BetName::DontCome)
    {
        if (amount < OddsTables::oddsDont[pivot_].denominator)
        {
            std::string s("CrapsBet::setOddsAmount(): odds bet is too small. "
                "Minimum odds bet for a " + EnumBetName::toString(betName_) +
                "(" + std::to_string(pivot_) + ") is " +
                std::to_string(OddsTables::oddsDont[pivot_].denominator) + ".");
            ep.diag = s;
            return Gen::ReturnCode::Fail;
        }
    }
    oddsAmount_ = amount;
    return Gen::ReturnCode::Success;    
}

/*-----------------------------------------------------------*//**

Evaluates a CrapsBet for win/lose.

Using the given point and the current dice roll, evaluate() determines
whether this bet has won, lost or not yet reached a decision. The
results of the evaluation are returned in a CrapsBet::DecisionRecord.

The DecisionRecord contains the following fields to convey handling by
the caller:

decision -indicates that a decision has been reached for this bet.

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

pivotAssigned - indicates whether a pivot for this bet has been freshly
established. For example, a Come bet has now been moved to the "6".

    When true, the pivot has been assigned. Caller uses this
    flag to move a come bet on the table to its box.

    When evaluating PassLine bets, if somehow a pivot is zero and the
    point has already been established, then the pivot is silently
    assigned to the already established point, as if the player made a
    PassLine bet after point was established.

win - if non-zero, then this bet has won the given amount. The
calculation includes odds winnings. The caller implementation should
take the win amount and add it with the original contractAmount and
oddsAmount to give back to the player.

lose - if non-zero, then this bet has lost the given amount. The amount
is just the contractBet plus odds bet, if any. The caller implementation
should add this amount to the table's banking system and subtract this
amount from the player's wallet if not already done so.

returnToPlayer - if non-zero then the given amount should be returned to
the player. Note this is not a win or lose. This happen when a
point-based bet wins/loses during a come out roll and the optional
setting of oddsBetOffComeOutRoll is true.

*/
Gen::ReturnCode
CrapsBet::evaluate(unsigned point, const Dice& dice,
                   DecisionRecord& dr, Gen::ErrorPass& ep)
{
//  diagEvalEntered(point, dice);  // TODO use debug conditional
    if (!validArgsEval(point, ep))
    {
        return diagEvalProcError(ep);
    }
                         
    dr = {betId_, false, 0,0,0};   // Prepare decision record
    Gen::ReturnCode rc;
    switch (betName_)
    {
    case BetName::PassLine: rc = evalPassLine(point, dice, dr, ep); break;
    case BetName::Come:     rc = evalCome    (point, dice, dr, ep); break;
    case BetName::DontPass: rc = evalDontPass(point, dice, dr, ep); break;
    case BetName::DontCome: rc = evalDontCome(point, dice, dr, ep); break;
    case BetName::Place   : rc = evalPlace   (point, dice, dr, ep); break;
    case BetName::Lay     : rc = evalLay     (point, dice, dr, ep); break;
    case BetName::Hardway : rc = evalHardway (point, dice, dr, ep); break;
    default: return Gen::ReturnCode::Success;
    }
    if (rc == Gen::ReturnCode::Fail)
    {
        return diagEvalProcError(ep);
    }
    distance_++;
    if (dr.decision)
    {
        whenDecided_ = std::chrono::system_clock::now();
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

void
CrapsBet::diagEvalEntered(unsigned point, const Dice& dice) const
{
    std::cout << "CrapsBet::evaluate(point:" << point
              << " dice:" << dice.value() << " (" 
              << dice.d1() << ","
              << dice.d2() << "))" << std::endl;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::diagEvalProcError(Gen::ErrorPass& ep) const
{
    std::string s("CrapsBet::evaluate(): Error evaluating betId:");
    s += std::to_string(betId_) + " betName:" + EnumBetName::toString(betName_) + ". ";
    Gen::StringUtils::prepend(ep.diag, s);
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

bool
CrapsBet::validArgsEval(unsigned point, Gen::ErrorPass& ep) const
{
    if (pointNums_.contains(point) || point == 0)
    {
        return true;
    }
    ep.diag = "Bad value for point:" + std::to_string(point);
    return false;
}

//----------------------------------------------------------------

void
CrapsBet::calcWinPointBet(
    unsigned d,
    DecisionRecord& dr,
    bool returnOdds,
    const OddsTables::OddsEntry table[]) const
{
    dr.win = contractAmount_;
    if (oddsAmount_ == 0) return;
    if (oddsBetOffComeOutRoll_ && returnOdds)
    {
        dr.returnToPlayer = oddsAmount_;
    }
    else
    {
        dr.win += (oddsAmount_ * table[d].numerator) / table[d].denominator;
    }
}

//----------------------------------------------------------------

void
CrapsBet::calcLossAnyBet(DecisionRecord& dr, bool returnOdds) const
{
    dr.lose = contractAmount_;
    if (oddsAmount_ == 0) return;
    if (oddsBetOffComeOutRoll_ && returnOdds)
    {
        dr.returnToPlayer = oddsAmount_;
    }
    else
    {
        dr.lose += oddsAmount_;
    }
}    

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalPassLine(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep; // unused, quiet the compiler
    Decision dcn = Keep;
    unsigned d = dice.value(); // Cache value once
    
    if (point == 0)
    {
        if (d == 7 || d == 11)
        {
            dcn = Win;
        }
        else if (crapsNums_.contains(d))
        {
            dcn = Lose;
        }
        else
        {
            pivot_ = d;
            dr.pivotAssigned = true;
            dcn = Keep;
        }
    }
    else
    {
        if (pivot_ == 0)
        {
            // Special case handing for pass bet. PassLine bet made
            // after come out roll, which is legal. Caller should have
            // have assigned it a pivot, but didn't so we'll do that
            // quietly.
            pivot_ = point;
            dr.pivotAssigned = true;
        }
        if (d == 7)
        {
            dcn = Lose;
        }
        else if (pivot_ == d)
        {
            dcn = Win;
        }
        // else dcn = Keep;
    }

    const bool returnOdds = false;
    if (dcn == Win)  calcWinPointBet(d, dr, returnOdds, OddsTables::oddsPass);
    if (dcn == Lose) calcLossAnyBet (dr, returnOdds);
    dr.decision = (dcn != Keep);
    return Gen::ReturnCode::Success;
}
    
//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalCome(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    if (point == 0 && pivot_ == 0)
    {
        // This bet is in the wrong state. It should not be on the
        // table. This means a Come bet was placed on the table
        // during a come out roll, which is illegal. Instead the
        // user must make a PassLine bet.
        //
        ep.diag = "Come bet is not allowed on come out roll. "
                  "Use PassLine bet instead.";
        return Gen::ReturnCode::Fail;
    }
        
    Decision dcn = Keep;
    bool returnOdds = false;
    unsigned d = dice.value(); // Cache value once
    
    if (point == 0)  // Come out roll (for the table)
    {
        if (d == 7)
        {
            dcn = Lose;
            returnOdds = true;
        }
        else if (bookEnds_.contains(d))
        {
            dcn = Keep;
        }
        else if (pivot_ == d)
        {
            dcn = Win;
            returnOdds = true;
        }
        // else dcn = Keep;
    }
    else
    {
        if (pivot_ == 0)  // Come bet freshly placed on table.
        {
            if (d == 7 || d == 11)
            {
                dcn = Win;
            }
            else if (crapsNums_.contains(d))
            {
                dcn = Lose;
            }
            else
            {
                pivot_ = d;
                dr.pivotAssigned = true;
                dcn = Keep;
            }
        }
        else   // Waiting for a repeating number.
        { 
            if (d == 7)
            {
                dcn = Lose;
            }
            else if (pivot_ == d)
            {
                dcn = Win;
            }
            // else dcn == keep;
        }
    }

    if (dcn == Win)  calcWinPointBet(d, dr, returnOdds, OddsTables::oddsPass);
    if (dcn == Lose) calcLossAnyBet (dr, returnOdds);
    dr.decision = (dcn != Keep);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalDontPass(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    Decision dcn = Keep;
    unsigned d = dice.value();  // Cache value once
    
    if ((point != 0) && (pivot_ == 0))
    {
        // This bet is in the wrong state, it should not be on the
        // table.  It means the DontPass bet was placed on the table
        // after the point was already established, which is illegal for
        // a DontPass bet. DontPass must sit through come roll
        // evaluation(s) in order to be assigned a pivot, thus if there
        // is a point, then pivot cannot be zero.
        //
        ep.diag = "DontPass bet is not allowed after point is established.";
        return Gen::ReturnCode::Fail;
    }

    if (point == 0)  // come out roll
    {
        if (d == 7 || d == 11)
        {
            dcn = Lose;
        }
        else if (d == 2 || d == 3)
        {
            dcn = Win;
        }
        else if (d == 12)
        {
            dcn = Keep;
        }
        else
        {
            pivot_ = d;
            dr.pivotAssigned = true;
            dcn = Keep;
        }
    }
    else
    {
        if (d == 7)
        {
            dcn = Win;
        }
        else if (pivot_ == d)
        {
            dcn = Lose;
        }
        // else dcn = Keep;
    }

    const bool returnOdds = false;
    if (dcn == Win)  calcWinPointBet(pivot_, dr, returnOdds, OddsTables::oddsDont);
    if (dcn == Lose) calcLossAnyBet (dr, returnOdds);
    dr.decision = (dcn != Keep);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalDontCome(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    if (point == 0 && pivot_ == 0)
    {
        // This bet is in the wrong state. It should not be on the
        // table. This means a DontCome bet was placed on the table
        // during a come out roll, which is illegal. Instead the
        // user must make a DontPass bet.
        //
        ep.diag = "DontCome bet is not allowed on Come out roll. "
                  " Use DontPass bet instead.";
        return Gen::ReturnCode::Fail;
    }
        
    Decision dcn = Keep;
    bool returnOdds = false;
    unsigned d = dice.value(); // Cache value once
    
    if (point == 0)  // Come out roll (for the table)
    {
        if (d == 7)
        {
            dcn = Win;
            returnOdds = true;
        }
        else if (bookEnds_.contains(d))
        {
            dcn = Keep;
        }
        else if (pivot_ == d)
        {
            dcn = Lose;
            returnOdds = true;
        }
        // else dcn = Keep;
    }
    else
    {
        if (pivot_ == 0)  // DontCome bet freshly placed on table.
        {
            if (d == 7 || d == 11)
            {
                dcn = Lose;
            }
            else if (d == 2 || d == 3)
            {
                dcn = Win;
            }
            else if (d == 12)
            {
                dcn = Keep;
            }
            else
            {
                pivot_ = d;
                dr.pivotAssigned = true;
                dcn = Keep;
            }
        }
        else   // Waiting for a 7 before number repeats.
        { 
            if (d == 7)
            {
                dcn = Win;
            }
            else if (pivot_ == d)
            {
                dcn = Lose;
            }
            // else dcn == keep;
        }
    }

    if (dcn == Win)  calcWinPointBet(pivot_, dr, returnOdds, OddsTables::oddsDont);
    if (dcn == Lose) calcLossAnyBet (dr, returnOdds);
    dr.decision = (dcn != Keep);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalPlace(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep;
    Decision dcn = Keep;
    
    if (point == 0 && placeBetOffComeOutRoll_)
    {
        dcn = Keep;
    }
    else
    {            
        if (dice.value() == 7)
        {
            dcn = Lose;
        }
        if (pivot_ == dice.value())
        {
            dcn = Win;
        }
        // else dcn = keep
    }
        
    if (dcn == Win)
    {
        dr.win = (contractAmount_ * OddsTables::oddsPlace[pivot_].numerator) /
            OddsTables::oddsPlace[pivot_].denominator;
    }
    if (dcn == Lose)
    {
        dr.lose = contractAmount_;
    }
    dr.decision = (dcn != Keep);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalLay(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep;
    Decision dcn = Keep;
    
    if (point == 0 && layBetOffComeOutRoll_)
    {
        dcn = Keep;
    }
    else
    {            
        if (dice.value() == 7)
        {
            dcn = Win;
        }
        if (pivot_ == dice.value())
        {
            dcn = Lose;
        }
        // else dcn = keep
    }
        
    if (dcn == Win)
    {
        dr.win = (contractAmount_ * OddsTables::oddsDont[pivot_].numerator) /
            OddsTables::oddsDont[pivot_].denominator;
        unsigned commission = static_cast<unsigned>(dr.win * (5.0f / 100.0f));
        dr.win -= commission;
        // TODO: dr.commission = commission;
    }
    if (dcn == Lose)
    {
        dr.lose = contractAmount_;
    }
    dr.decision = (dcn != Keep);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalHardway(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep; (void) point;  // unused, quiet the compiler
    Decision dcn = Keep;

    if (hardwayBetOff_)
    {
        dcn = Keep;
    }
    else if (dice.value() == 7)
    {
        dcn = Lose;
    }
    else if (dice.value() == pivot_)
    {
        if (dice.d1() == dice.d2())
        {
            dcn = Win;
        }
        else
        {
            dcn = Lose;
        }
    }
    // else dcn = Keep
    
    if (dcn == Win)
    {
        dr.win = (contractAmount_ * OddsTables::oddsHardway[pivot_].numerator) /
            OddsTables::oddsHardway[pivot_].denominator;
    }
    if (dcn == Lose)
    {
        dr.lose = contractAmount_;
    }
    dr.decision = (dcn != Keep);
    return Gen::ReturnCode::Success;
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
CrapsBet::oddsBetOffComeOutRoll() const
{
    return oddsBetOffComeOutRoll_;
}

/*-----------------------------------------------------------*//**

Returns whether the place bet is off on the come out roll.

@return
    true if place bet is off on the come out roll, false otherwise.

*/
bool
CrapsBet::placeBetOffComeOutRoll() const
{
    return placeBetOffComeOutRoll_;
}

/*-----------------------------------------------------------*//**

Returns whether the lay bet is off on the come out roll.

@return
    true if lay bet is off on the come out roll, false otherwise.

*/
bool
CrapsBet::layBetOffComeOutRoll() const
{
    return layBetOffComeOutRoll_;
}

/*-----------------------------------------------------------*//**

Returns whether the hardway bet is off or working.

@return
    true if hardway bet is off, false otherwise.

*/
bool
CrapsBet::hardwayBetOff() const
{
    return hardwayBetOff_;
}

/*-----------------------------------------------------------*//**

Disable the odds bet for this bet on come out rolls.

Odds bets on come out rolls are off by default.

*/
void
CrapsBet::setOddsBetOffComeOutRoll()
{
    oddsBetOffComeOutRoll_ = true;
}

/*-----------------------------------------------------------*//**

Enable the odds bet for this bet on come out rolls.

Odds bets on come out rolls are off by default.

*/
void
CrapsBet::setOddsBetWorkingComeOutRoll()
{
    oddsBetOffComeOutRoll_ = false;
}

/*-----------------------------------------------------------*//**

Disable this place bet on come out rolls.

Place bets on come out rolls are off by default.

*/
void
CrapsBet::setPlaceBetOffComeOutRoll()
{
    placeBetOffComeOutRoll_ = true;
}

/*-----------------------------------------------------------*//**

Enable this place bet on come out rolls.

Place bets on come out rolls are off by default. This
can enable it.

*/
void
CrapsBet::setPlaceBetWorkingComeOutRoll()
{
    placeBetOffComeOutRoll_ = false;
}

/*-----------------------------------------------------------*//**

Disable this lay bet on come out rolls.

Lay bets on come out rolls are off by default.

*/
void
CrapsBet::setLayBetOffComeOutRoll()
{
    layBetOffComeOutRoll_ = true;
}

/*-----------------------------------------------------------*//**

Enable this lay bet on come out rolls.

Lay bets on come out rolls are off by default. This
can enable it.

*/
void
CrapsBet::setLayBetWorkingComeOutRoll()
{
    layBetOffComeOutRoll_ = false;
}

/*-----------------------------------------------------------*//**

Enable this hardway bet.

Hardway bets are working by default.

*/
void
CrapsBet::setHardwayBetWorking()
{
    hardwayBetOff_ = false;
}

/*-----------------------------------------------------------*//**

Disable this hardway bet.

Hardway bets are working by default.

*/
void
CrapsBet::setHardwayBetOff()
{
    hardwayBetOff_ = true;
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

std::ostream&
operator<< (std::ostream& out, const CrapsBet& b)
{
    out <<
    "             betId: " << b.betId()                 << std::endl <<
    "           betName: " << b.betName()               << std::endl <<
    "             pivot: " << b.pivot()                 << std::endl <<
    "    contractAmount: " << b.contractAmount()        << std::endl <<
    "        oddsAmount: " << b.oddsAmount()            << std::endl <<
    "oddsOffComeOutRoll: " << b.oddsBetOffComeOutRoll() << std::endl <<
    "          distance: " << b.distance()              << std::endl <<
    "       whenCreated: " << b.whenCreated()           << std::endl <<
    "       whenDecided: " << b.whenDecided()           << std::endl;
    return out;
}

//----------------------------------------------------------------

std::ostream&
operator<< (std::ostream& out, const CrapsBet::DecisionRecord& dr)
{
    out <<
    "         betId: " << dr.betId          << std::endl <<
    "      decision: " << dr.decision       << std::endl <<
    " pivotAssigned: " << dr.pivotAssigned  << std::endl <<
    "           win: " << dr.win            << std::endl <<
    "          lose: " << dr.lose           << std::endl <<
    "returnToPlayer: " << dr.returnToPlayer << std::endl;
    return out;
}

//----------------------------------------------------------------
