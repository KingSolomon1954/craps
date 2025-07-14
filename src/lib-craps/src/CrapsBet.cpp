//----------------------------------------------------------------
//
// File: CrapsBet.cpp
//
//----------------------------------------------------------------

#include <craps/CrapsBet.h>

#include <stdexcept> // for std::invalid_argument
#include <iostream>
#include <craps/DecisionRecord.h>
#include <craps/Dice.h>
#include <gen/ErrorPass.h>
#include <gen/StringUtils.h>

using namespace Craps;

unsigned CrapsBet::idCounter_ = 0;

const std::unordered_set<unsigned> CrapsBet::pointNums_   = {4, 5, 6, 8, 9, 10};
const std::unordered_set<unsigned> CrapsBet::fieldNums_   = {2, 3, 4, 9, 10, 11, 12};
const std::unordered_set<unsigned> CrapsBet::crapsNums_   = {2, 3, 12};
const std::unordered_set<unsigned> CrapsBet::bookEnds_    = {2, 3, 11, 12};
const std::unordered_set<unsigned> CrapsBet::hardwayNums_ = {4, 6, 8, 10};

/*-----------------------------------------------------------*//**

Constructor - creates a CrapsBet

Creates a Craps bet. The bet is given a unique ID and time of creation.
The CrapsBet class does not store or use betIds for any purpose. betId
is meant for upper level designs that track many bets.

Arguments are checked for validity and if found to be bad then
std::invalid_argument is thrown.

Note that no check is made at this time whether the bet is allowed to be
placed on a Craps table. CrapsBet is unaware of the state of any Craps
table. This decouples the act of creating a bet from placing a bet on a
CrapsTable, allowing the application better freedom of design.

@param[in] playerId
    The player making this bet.

@param[in] name
    The name/type of the bet. Throws if this is not valid name.

@param[in] contractAmount
    The amount of the bet. Throws if this is zero.
    For C&E bets, min bet is 2 must be a multiple of 2.
    For Horn bets, min bet is 4 must be a multiple of 4.

@param[in] pivot
    The number this bet is focused on. For example a Place bet must set
    the pivot to 4,5,6,8,9 or 10 otherwise a std::invalid_argument
    exception is thrown.

    For Field, AnyCraps, AnySeven, C&E and Horn bets, the pivot is
    unused and internally set to 0.

    For PassLine/Come/DontPass/DontCome bets, the caller sets the pivot
    to zero. Zero indicates the pivot number needs to be set later.
    These point-based bets get their pivot number assigned during
    CrapsBet::evaluate(),

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
*/
CrapsBet::CrapsBet(
    const Gen::Uuid& playerId,
    BetName name,
    Gen::Money contractAmount,
    unsigned pivot)
    : playerId_(playerId)
    , betId_(++idCounter_)
    , betName_(name)
    , pivot_(pivot)
    , contractAmount_(contractAmount)
    , oddsAmount_(0)
    , offComeOutRoll_(true)
    , distance_(0)
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
        offComeOutRoll_ = false;  // For hardways, default is  always working
        if (!hardwayNums_.contains(pivot_))
        {
            throw std::invalid_argument(
                "CrapsBet()::ctor Bad \"pivot\": for Hardway "
                "bet, pivot must be one of 4,6,8,10");
        }
    }
    if (betName_ == BetName::Field    ||
        betName_ == BetName::AnyCraps ||
        betName_ == BetName::AnySeven ||
        betName_ == BetName::CandE    ||
        betName_ == BetName::Horn)
    {
        pivot_ = 0;  // Quietly force it to 0.
    }
    if (betName_ == BetName::CandE && (contractAmount_ % 2 != 0))
    {
        throw std::invalid_argument(
            "CrapsBet()::ctor C&E bet(" +
            std::to_string(contractAmount_) +
            ") is not a multiple of 2. Min amount is 2");
    }
    if (betName_ == BetName::Horn && (contractAmount_ % 4 != 0))
    {
        throw std::invalid_argument(
            "CrapsBet()::ctor Horn bet(" +
            std::to_string(contractAmount_) +
            ") is not a multiple of 4. Min amount is 4");
    }
}

//----------------------------------------------------------------

bool
CrapsBet::operator==(const CrapsBet& other) const
{
    return playerId_       == other.playerId_       &&
           betId_          == other.betId_          &&
           pivot_          == other.pivot_          &&
           contractAmount_ == other.contractAmount_ &&
           oddsAmount_     == other.oddsAmount_     &&
           offComeOutRoll_ == other.offComeOutRoll_ &&
           distance_       == other.distance_       &&
           whenCreated_    == other.whenCreated_    &&
           whenDecided_    == other.whenDecided_;    
} 

/*-----------------------------------------------------------*//**

Sets the contract bet to the new amount.

*/
Gen::ReturnCode
CrapsBet::setContractAmount(Gen::Money amount, Gen::ErrorPass& ep)
{
    if (amount == 0)
    {
        ep.diag = "New contract amount would be zero or less. "
            "Use removeBet() if intent is to pull the bet.";
        return Gen::ReturnCode::Fail;
    }
    
    if (betName_ == BetName::PassLine ||
        betName_ == BetName::Come)
    {
        if (pivot_ != 0 && amount < contractAmount_)
        {
            ep.diag = "Cannot reduce contract amount for PassLine/Come "
                      "bets after point is established.";
            return Gen::ReturnCode::Fail;
        }
    }

    if (betName_ == BetName::DontPass ||
        betName_ == BetName::DontCome)
    {
        if (pivot_ != 0 && amount > contractAmount_)
        {
            ep.diag = "Cannot increase contract amount for DontPass/DontCome "
                      "bets after point is established.";
            return Gen::ReturnCode::Fail;
        }
    }
    contractAmount_ = amount;
    return Gen::ReturnCode::Success;
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
CrapsBet::setOddsAmount(Gen::Money newAmount, Gen::ErrorPass& ep)
{
    if (betName_ != BetName::PassLine && betName_ != BetName::DontPass &&
        betName_ != BetName::Come     && betName_ != BetName::DontCome)
    {
        std::string s("Odds bet is only available for "
            "PassLine|Come|DontPass|DontCome bets. Current bet is betId:");
        s += std::to_string(betId_) + " betName:" +
            EnumBetName::toString(betName_) + ". ";
        ep.diag = s;
        return Gen::ReturnCode::Fail;
    }

    // OK to remove odds bet anytime.
//  if (amount == 0 && oddsAmount_ > 0)
    if (newAmount == 0)
    {
        oddsAmount_ = 0;
        return Gen::ReturnCode::Success;
    }

    // Otherwise, caller is adding or changing amount

    if (pivot_ == 0)
    {
        std::string s("Odds bet is not allowed. "
            "Need a point to be established for this bet. ");
        s += "betId:" + std::to_string(betId_) + " betName:" +
            EnumBetName::toString(betName_) + " pivot:" +
            std::to_string(pivot_) + ".";
        ep.diag = s;
        return Gen::ReturnCode::Fail;
    }

    if (betName_ == BetName::PassLine || betName_ == BetName::Come)
    {
        if (newAmount < OddsTables::oddsPass[pivot_].denominator)
        {
            std::string s("Odds bet amount of ");
            s += std::to_string(newAmount) + " is too small. "
                "Minimum odds bet for a " + EnumBetName::toString(betName_) +
                "(" + std::to_string(pivot_) + ") is " +
                std::to_string(OddsTables::oddsPass[pivot_].denominator) + ".";
            ep.diag = s;
            return Gen::ReturnCode::Fail;
        }
    }
    if (betName_ == BetName::DontPass || betName_ == BetName::DontCome)
    {
        if (newAmount < OddsTables::oddsDont[pivot_].denominator)
        {
            std::string s("Odds bet amount of ");
            s += std::to_string(newAmount) + " is too small. "
                "Minimum odds bet for a " + EnumBetName::toString(betName_) +
                "(" + std::to_string(pivot_) + ") is " +
                std::to_string(OddsTables::oddsDont[pivot_].denominator) + ".";
            ep.diag = s;
            return Gen::ReturnCode::Fail;
        }
    }
    oddsAmount_ = newAmount;
    return Gen::ReturnCode::Success;    
}

/*-----------------------------------------------------------*//**

Evaluates a CrapsBet for win/lose.

Using the given point and the current dice roll, evaluate() determines
whether this bet has won, lost or not yet reached a decision. The
results of the evaluation are returned in a CrapsBet::DecisionRecord.

@param [in] point
    supply 0 to indicate a come out roll, otherwise the current point
    which must be one of 4,5,6,8,9 or 10. If not then an error is
    returned and ep holds the reason.

@param [in] dice
    the dice roll to evaluate

@param [inout] dr
    The DecisionRecord contains the results of the evaluation.
    If successful then dr has valid contents, otherwise
    dr should be ignored and ep has description of the error.

@param [inout] ep
    If evaluation fails, then ep describes the error.

*/

/*-----------------------------------------------------------*//**

@struct CrapsBet::DecisionRecord

The DecisionRecord contains the following fields to convey handling by
the caller:

@betId - the id of the bet associated with this result

@decision -indicates that a decision has been reached for this bet.

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

@pivotAssigned - indicates whether a pivot for this bet has been freshly
    established. For example, a Come bet has now been moved to the "6".

    When true, the pivot has been assigned. Caller uses this
    flag to move a come bet on the table to its box.

    When evaluating PassLine bets, if somehow a pivot is zero and the
    point has already been established, then the pivot is silently
    assigned to the already established point, as if the player made a
    PassLine bet after point was established.

@win - if non-zero, then this bet has won the given amount.

    The calculation includes odds winnings. The caller implementation
    should take this win amount and add it with the original
    contractAmount and oddsAmount to give back to the player.

@lose - if non-zero, then this bet has lost the given amount.

    The amount is the contractBet plus odds bet, if any. The caller
    implementation should add this amount to the table's banking system
    and subtract this amount from the player's wallet if not already
    done so.

@returnToPlayer - if non-zero then the given amount should be returned
    to the player.

    Note this is not a win or lose. This happens when a point-based bet
    wins/loses during a come out roll and setOffComeOutRoll() is true.

@commission - Lay and Buy bets require a 5% commission if they win.

    This amount has been subtracted from the win amount.  The table's
    banking system should deposit this amount if non-zero.
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
                         
    dr = {betId_, false, 0,0,0,0, 0, playerId_};   // Prepare decision record
    Gen::ReturnCode rc = Gen::ReturnCode::Fail;
    switch (betName_)
    {
        case BetName::PassLine: rc = evalPassLine(point, dice, dr, ep); break;
        case BetName::Come    : rc = evalCome    (point, dice, dr, ep); break;
        case BetName::DontPass: rc = evalDontPass(point, dice, dr, ep); break;
        case BetName::DontCome: rc = evalDontCome(point, dice, dr, ep); break;
        case BetName::Place   : rc = evalPlace   (point, dice, dr, ep); break;
        case BetName::Buy     : rc = evalBuy     (point, dice, dr, ep); break;
        case BetName::Lay     : rc = evalLay     (point, dice, dr, ep); break;
        case BetName::Hardway : rc = evalHardway (point, dice, dr, ep); break;
        case BetName::Field   : rc = evalField   (point, dice, dr, ep); break;
        case BetName::AnyCraps: rc = evalAnyCraps(point, dice, dr, ep); break;
        case BetName::AnySeven: rc = evalAnySeven(point, dice, dr, ep); break;
        case BetName::CandE   : rc = evalCandE   (point, dice, dr, ep); break;
        case BetName::Horn    : rc = evalHorn    (point, dice, dr, ep); break;
        case BetName::Count   : // fall through
        case BetName::Invalid : return Gen::ReturnCode::Fail;           break;
    }
    if (rc == Gen::ReturnCode::Fail)
    {
        return diagEvalProcError(ep);
    }
    distance_++;
    if (dr.decision)
    {
        whenDecided_.setToNow();
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
    if (offComeOutRoll_ && returnOdds)
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
CrapsBet::calcLossPointBet(DecisionRecord& dr, bool returnOdds) const
{
    dr.lose = contractAmount_;
    if (oddsAmount_ == 0) return;
    if (offComeOutRoll_ && returnOdds)
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
    if (dcn == Lose) calcLossPointBet (dr, returnOdds);
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
    if (dcn == Lose) calcLossPointBet (dr, returnOdds);
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
    if (dcn == Lose) calcLossPointBet (dr, returnOdds);
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
    if (dcn == Lose) calcLossPointBet (dr, returnOdds);
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
    
    if (point == 0 && offComeOutRoll_)
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
    
    if (point == 0 && offComeOutRoll_)
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
        dr.commission = static_cast<unsigned>(dr.win * (5.0f / 100.0f));
        dr.win -= dr.commission;
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
CrapsBet::evalBuy(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep;
    Decision dcn = Keep;
    
    if (point == 0 && offComeOutRoll_)
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
        dr.win = (contractAmount_ * OddsTables::oddsPass[pivot_].numerator) /
            OddsTables::oddsPass[pivot_].denominator;
        dr.commission = static_cast<unsigned>(contractAmount_ * (5.0f / 100.0f));
        dr.win -= dr.commission;
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
CrapsBet::evalField(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep; (void) point;  // unused, quiet the compiler
    Decision dcn = Keep;
    unsigned d = dice.value();  // cache value once
    
    // For field bets, in additon to win/lose we also set the pivot. 
    // Allows us to keeps stats on field bet win/lose per number.

    if (d == 5 || d == 6 || d == 7 || d == 8) 
    {
        dcn = Lose;
        pivot_ = d;
    }
    else
    {
        dcn = Win;
        pivot_ = d;
    }
        
    if (dcn == Win)
    {
        dr.win = contractAmount_;
        if (d == 2 || d == 12) dr.win += contractAmount_;  // pays double
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

    if (offComeOutRoll_)
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

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalAnyCraps(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep; (void) point;  // unused, quiet the compiler
    
    if (crapsNums_.contains(dice.value()))
    {
        dr.win = contractAmount_ * 7;
    }
    else
    {
        dr.lose = contractAmount_;
    }
    dr.decision = true;
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalAnySeven(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep; (void) point;  // unused, quiet the compiler
    
    if (dice.value() == 7)
    {
        dr.win = contractAmount_ * 4;
    }
    else
    {
        dr.lose = contractAmount_;
    }
    dr.decision = true;
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalCandE(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep; (void) point;  // unused, quiet the compiler
    
    if (dice.value() == 11)
    {
        dr.win = (contractAmount_ / 2) * 15;
    }
    else if (crapsNums_.contains(dice.value()))
    {
        dr.win = (contractAmount_ / 2) * 7;
    }
    else
    {
        dr.lose = contractAmount_;
    }
    dr.decision = true;
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsBet::evalHorn(
    unsigned point,
    const Dice& dice,
    DecisionRecord& dr,
    Gen::ErrorPass& ep)    
{
    (void) ep; (void) point;  // unused, quiet the compiler

    if (dice.value() == 2 || dice.value() == 12) 
    {
        dr.win = (contractAmount_ / 4) * 30;
    }
    else if (dice.value() == 3 || dice.value() == 11) 
    {
        dr.win = (contractAmount_ / 4) * 15;
    }
    else
    {
        dr.lose = contractAmount_;
    }
    dr.decision = true;
    return Gen::ReturnCode::Success;
}

/*-----------------------------------------------------------*//**

Returns the playerId.

@return
    The playerId
*/
const Gen::Uuid&
CrapsBet::playerId() const
{
    return playerId_;
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

Returns whether the bet is "working/on" or "not-working/off" on a
come out roll.

For point bets (PassLine|Come|DontPass|DontCome), this refers to the
odds amount.

@return
    true if "off" on come out roll, false otherwise.
*/
bool
CrapsBet::offComeOutRoll() const
{
    return offComeOutRoll_;
}

/*-----------------------------------------------------------*//**

Returns whether Hardway bet is "working/on" or "not-working/off".

@return
    true if "working", false otherwise.
*/
bool
CrapsBet::hardwayWorking() const
{
    return !offComeOutRoll_;
}

/*-----------------------------------------------------------*//**

Set this bet to be off on come out rolls.

Off is the default,
*/
void
CrapsBet::setOffComeOutRoll()
{
    offComeOutRoll_ = true;
}

/*-----------------------------------------------------------*//**

Set this bet to be working on come out rolls.

Off is the default.
*/
void
CrapsBet::setOnComeOutRoll()
{
    offComeOutRoll_ = false;
}

/*-----------------------------------------------------------*//**

Set this Hardway bet to "working".

Working is the default for a Hardway bet. 

If this bet is not a Hardway bet, then just silently returns.
*/
void
CrapsBet::setHardwayOn()
{
    if (betName_ != BetName::Hardway) return;
    offComeOutRoll_ = true;
}

/*-----------------------------------------------------------*//**

Set this Hardway bet to "off".

Working is the default for a Hardway bet.

If this bet is not a Hardway bet, then just silently returns.
*/
void
CrapsBet::setHardwayOff()
{
    if (betName_ != BetName::Hardway) return;
    offComeOutRoll_ = false;
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
Gen::Timepoint
CrapsBet::whenCreated() const
{
    return whenCreated_;
}

/*-----------------------------------------------------------*//**

Returns the time the bet reached a decision.

@return
    time of bet decision.
*/
Gen::Timepoint
CrapsBet::whenDecided() const
{
    return whenDecided_;
}

//----------------------------------------------------------------

std::ostream&
operator<< (std::ostream& out, const CrapsBet& b)
{
    out <<
    "      playerId: " << b.playerId()       << std::endl <<
    "         betId: " << b.betId()          << std::endl <<
    "       betName: " << b.betName()        << std::endl <<
    "         pivot: " << b.pivot()          << std::endl <<
    "contractAmount: " << b.contractAmount() << std::endl <<
    "    oddsAmount: " << b.oddsAmount()     << std::endl <<
    "offComeOutRoll: " << b.offComeOutRoll() << std::endl <<
    "      distance: " << b.distance()       << std::endl <<
    "   whenCreated: " << b.whenCreated()    << std::endl <<
    "   whenDecided: " << b.whenDecided()    << std::endl;
    return out;
}

//----------------------------------------------------------------

std::ostream&
operator<< (std::ostream& out, const DecisionRecord& dr)
{
    out <<
    "      playerId: " << dr.playerId       << std::endl <<
    "         betId: " << dr.betId          << std::endl <<
    "      decision: " << dr.decision       << std::endl <<
    " pivotAssigned: " << dr.pivotAssigned  << std::endl <<
    "           win: " << dr.win            << std::endl <<
    "          lose: " << dr.lose           << std::endl <<
    "returnToPlayer: " << dr.returnToPlayer << std::endl;
    return out;
}

//----------------------------------------------------------------
