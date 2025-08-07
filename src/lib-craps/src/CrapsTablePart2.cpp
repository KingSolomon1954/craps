//----------------------------------------------------------------
//
// File: CrapsTablePart2.cpp
//
// Continuation of CrapsTable
// 
//----------------------------------------------------------------

#include <craps/CrapsTable.h>
#include <cassert>
#include <craps/CrapsBet.h>
#include <gen/Logger.h>

using namespace Craps;

//----------------------------------------------------------------

std::string
CrapsTable::diagPrefix(size_t idx, const CrapsBet& bet) const
{
    std::string diag("CrapsTable::");
    if (idx == 1)
    {
        diag += "addBet(): Unable to add ";
    }
    if (idx == 2)
    {
        diag += "setContractAmount(): Unable to set contract amount for ";
    }
    if (idx == 3)
    {
        diag += "setOddsAmount(): Unable to set odds amount for ";
    }
    if (idx > 3) assert(false);

    return diag + bet.diagBetId() + "; ";
}

//----------------------------------------------------------------

bool
CrapsTable::fifBettingClosed(const CrapsBet& bet, size_t idx, Gen::ErrorPass& ep) const
{
    // fault if betting is closed and sets ep error diag
    if (!bettingOpen_)
    {
        ep.diag = diagPrefix(1, bet) + "Betting is closed at the moment - "
                                       "dice roll is underway.";
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifMissingPlayer(const CrapsBet& bet, Gen::ErrorPass& ep) const
{        
    // fault if player is missing and sets ep error diag
    if (!havePlayer(&bet.player()))
    {
        ep.diag = diagPrefix(1, bet) + "Player XXX is not joined with this table.";
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifHaveBet(const CrapsBet& bet, Gen::ErrorPass& ep) const
{
    // fault if already have bet and sets ep error diag
    if (haveBet(bet))
    {
        ep.diag = diagPrefix(1, bet) + "Player XXX has already made this bet.";
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::fifMissingBet(const CrapsBet& bet, size_t idx, Gen::ErrorPass& ep) const
{
    // fault if bet is not on the table and sets ep error diag
    if (!haveBet(bet))
    {
        ep.diag = diagPrefix(idx, bet) + "This bet is not on the table.";
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::fifZeroAmount(const CrapsBet& bet,
                          Gen::Money amt,
                          size_t idx,
                          Gen::ErrorPass& ep) const
{
    if (amt == 0)
    {
        ep.diag = diagPrefix(idx, bet) + "Bet cannot be $0.";
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifComeDisallowed(const CrapsBet& bet, Gen::ErrorPass& ep) const
{
    // fault if come/dontcome bet is disallowed and sets ep error diag
    if (bet.betName() == BetName::Come || bet.betName() == BetName::DontCome)
    {
        if (point_ == 0)
        {
            ep.diag = diagPrefix(1, bet) + "This bet is not allowed "
                                           "during come out roll.";
            return true;
        }
        if (bet.pivot() != 0)
        {
            // Not a come out roll, but can't have a pivot already assigned
            ep.diag = diagPrefix(1, bet) + "This bet cannot specify " +
                "a point/pivot of " + std::to_string(bet.pivot()) +
                ". Pivot must be 0.";
            return true;
        }
    }
    return false;
}
    
//----------------------------------------------------------------

bool
CrapsTable::fifDontPassDisallowed(const CrapsBet& bet, Gen::ErrorPass& ep) const
{
    // fault if DontPass is disallowed and sets ep error diag
    if (bet.betName() == BetName::DontPass && point_ != 0)
    {
        ep.diag = diagPrefix(1, bet) + 
            "This bet is not allowed while there is already a point.";
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifBadPassLineChange(const CrapsBet& bet, Gen::Money amt,
                                 size_t idx, Gen::ErrorPass& ep) const
{
    if (bet.betName() == BetName::PassLine || bet.betName() == BetName::Come)
    {
        if (bet.pivot() != 0 && amt < bet.contractAmount())
        {
            ep.diag = diagPrefix(2, bet) +
                "Cannot reduce contract amount for PassLine/Come "
                "bets after point is established.";
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifBadDontPassChange(const CrapsBet& bet, Gen::Money amt,
                                 size_t idx, Gen::ErrorPass& ep) const
{
    if (bet.betName() == BetName::DontPass || bet.betName() == BetName::DontCome)
    {
        if (bet.pivot() != 0)
        {
            if (amt > bet.contractAmount())
            {
                ep.diag = diagPrefix(2, bet) +
                    "Cannot increase contract amount "
                    "for DontPass/DontCome bets after point is established.";
                return true;
            }
            else
            {
                // Reducing bet OK; but have to deal with odds
                if (bet.oddsAmount() > (amt * maxLineBet_))
                {
                    ep.diag = diagPrefix(2, bet) + 
                    "Cannot reduce contract bet due to large odds bet. First"
                    "reduce odds amount before reducing contract amount, "
                    "otherwise odds bet would exceed table limits.";
                    return true;
                }
            }
        }
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifBadMinMaxLineBets(const CrapsBet& bet, Gen::Money amt,
                                 size_t idx, Gen::ErrorPass& ep) const
{
    // fault if exceeded limits and sets ep error diag
    if (!withinMinMaxLineBets(bet.betName(), amt, ep))
    {
        ep.prepend(diagPrefix(idx, bet));
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifBadMinMaxSideBets(const CrapsBet& bet, Gen::Money amt,
                                 size_t idx, Gen::ErrorPass& ep) const
{
    // fault if exceeded limits and sets ep error diag
    if (!withinMinMaxSideBets(bet.betName(), bet.contractAmount(), ep))
    {
        ep.prepend(diagPrefix(idx, bet));
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifBadMultiples(const CrapsBet& bet, Gen::Money amt,
                            size_t idx, Gen::ErrorPass& ep) const
{
    // fault if not a proper multiple and sets ep error diag
    if (!goodMultiplesSideBets(bet.betName(), amt, ep))
    {
        ep.prepend(diagPrefix(idx, bet));
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifBadBetTypeForOdds(const CrapsBet& bet, Gen::ErrorPass& ep) const
{
    // fault if Odds bet not allowed for this bet type, sets up error diag
    BetName betName = bet.betName();
    if (betName != BetName::PassLine && betName != BetName::DontPass &&
        betName != BetName::Come     && betName != BetName::DontCome)
    {
        std::string s("Odds bet is only available for "
                      "PassLine|Come|DontPass|DontCome bets.");
        ep.diag = diagPrefix(3, bet) + s;
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifZeroPivotForOdds(const CrapsBet& bet, Gen::ErrorPass& ep) const
{
    // fault if zero pivot and sets ep error diag
    if (bet.pivot() == 0)
    {
        std::string s(" Odds bet is only allowed after a "
            "point has been established for this bet.");
        ep.diag = diagPrefix(3, bet) + s;
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::fifBadMinMaxForOdds(const CrapsBet& bet, Gen::Money amt,
                                Gen::ErrorPass& ep) const
{
    // fault if odds amount is out of range and sets ep error diag
    
    // Minimum odds is zero. But no need to test for that.
    if (amt > (bet.contractAmount() * maxOdds_))
    {
        ep.diag = diagPrefix(3, bet) + " Exceeds table limit of "      +
            std::to_string(maxOdds_) + "x odds; "
            "Contract amount is "                                      +
            Gen::MoneyUtils::toString(bet.contractAmount())            +
            " which allows max odds amount of "                        +
            Gen::MoneyUtils::toString(bet.contractAmount() * maxOdds_) +  ".";
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::withinMinMaxLineBets(BetName betName,
                                 Gen::Money amount,
                                 Gen::ErrorPass& ep) const
{
    if (betName == BetName::PassLine ||
        betName == BetName::Come     ||     
        betName == BetName::DontPass ||
        betName == BetName::DontCome ||
        betName == BetName::Place    ||
        betName == BetName::Buy      ||
        betName == BetName::Lay)
    {
        if (amount < minLineBet_ || amount > maxLineBet_)
        {
            std::string min = std::to_string(minLineBet_);
            std::string max = std::to_string(maxLineBet_);
            ep.diag = diagLimits(amount) +
              "(min:" + min + ",max:" + max + ").";
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------

bool
CrapsTable::withinMinMaxSideBets(BetName betName,
                                 Gen::Money amt,
                                 Gen::ErrorPass& ep) const
{
    Gen::Money min, max = 0;

    if (betName == BetName::Field && (amt < minFieldBet_ || amt > maxFieldBet_))
    {
        min = minFieldBet_; max = maxFieldBet_;
    }
    if (betName == BetName::CandE && (amt < minCandEBet_ || amt > maxCandEBet_))
    {
        min = minCandEBet_; max = maxCandEBet_;
    }
    if (betName == BetName::Horn && (amt < minHornBet_ || amt > maxHornBet_))
    {
        min = minHornBet_; max = maxHornBet_;
    }
    if (min != 0 || max != 0)
    {
        std::string minStr = std::to_string(min);
        std::string maxStr = std::to_string(max);
            ep.diag = diagLimits(amt) +
              "(min:" + minStr + ",max:" + maxStr + ").";
            return false;
    }
    return true;
}

//----------------------------------------------------------------

bool
CrapsTable::goodMultiplesSideBets(BetName betName,
                          Gen::Money amt,
                          Gen::ErrorPass& ep) const
{
    if (betName == BetName::CandE && (amt % 2 != 0))
    {
        ep.diag = "This bet must be a multiple of 2. " +
                  Gen::MoneyUtils::toString(amt)        +
                  " is not a multiple of 2.";
        return false;
    }
    if (betName == BetName::Horn && (amt % 4 != 0))
    {
        ep.diag = "This bet must be a multiple of 4. " +
                  Gen::MoneyUtils::toString(amt)        +
                  " is not a multiple of 4.";
        return false;
    }
    return true;
}

//----------------------------------------------------------------

std::string
CrapsTable::diagLimits(Gen::Money amt) const
{
    return "Bad bet amount:$" + std::to_string(amt) +
        " is outside of table limit ";
}

//----------------------------------------------------------------

