//----------------------------------------------------------------
//
// File: CrapsTablePart2.cpp
//
// Continuation of CrapsTable
// 
//----------------------------------------------------------------

#include <craps/CrapsTable.h>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <craps/Player.h>
#include <cassert>
#include <craps/CrapsBet.h>
#include <gen/Logger.h>

using namespace Craps;

//----------------------------------------------------------------

BetAmountLimits
CrapsTable::getBetAmountLimits(BetName betName,
                              unsigned pivot,
                              bool oddsBet,
                              Gen::Money contractAmount) const
{
    BetAmountLimits limits;
    const auto toMoney = [](std::uint64_t value)
    {
        return static_cast<Gen::Money>(std::min<std::uint64_t>(
            value, std::numeric_limits<Gen::Money>::max()));
    };

    if (oddsBet)
    {
        limits.increment = rules_.odds.wagerIncrement;
        limits.minimum = limits.increment;
        if (contractAmount == 0 ||
            (pivot != 4 && pivot != 5 && pivot != 6 &&
             pivot != 8 && pivot != 9 && pivot != 10))
            return limits;

        const unsigned multiplier =
            rules_.odds.style == TableRules::OddsLimitStyle::FlatMultiplier
                ? rules_.odds.flatMultiplier
                : rules_.odds.pointMultipliers[pivot];
        limits.maximum = toMoney(
            static_cast<std::uint64_t>(contractAmount) * multiplier);
        if (rules_.odds.maximumWager > 0)
            limits.maximum = std::min(limits.maximum, rules_.odds.maximumWager);

        if (rules_.odds.maximumPotentialWin > 0)
        {
            const auto& payout =
                (betName == BetName::DontPass || betName == BetName::DontCome)
                    ? OddsTables::oddsDont[pivot] : OddsTables::oddsPass[pivot];
            if (payout.numerator == 0)
                limits.maximum = 0;
            else
            {
                const std::uint64_t cap = rules_.odds.maximumPotentialWin;
                const std::uint64_t numerator = payout.numerator;
                const std::uint64_t denominator = payout.denominator;
                const auto maxByWin = static_cast<Gen::Money>(
                    (((cap + 1) * denominator) - 1) / numerator);
                limits.maximum = std::min(limits.maximum, maxByWin);
            }
        }
        // The permitted amount must also satisfy the configured wager increment.
        limits.maximum -= limits.maximum % limits.increment;
        return limits;
    }

    const auto& wager = rules_.wagerLimit(betName);
    limits.minimum = wager.minimum;
    limits.maximum = wager.maximum;
    limits.increment = wager.increment;

    unsigned components = 0;
    switch (betName)
    {
        case BetName::CandE: components = 2; break;
        case BetName::Horn:  components = 4; break;
        case BetName::World: components = 5; break;
        default: break;
    }
    if (components != 0)
    {
        const auto& aggregate = rules_.aggregateLimit(betName);
        if (aggregate.maximumComponentWager > 0)
            limits.maximum = std::min(limits.maximum, toMoney(
                static_cast<std::uint64_t>(aggregate.maximumComponentWager) * components));
        if (aggregate.basis == TableRules::LimitBasis::PotentialWin &&
            aggregate.maximumPotentialWin > 0)
        {
            std::uint64_t numerator = 0;
            std::uint64_t denominator = 1;
            if (betName == BetName::CandE) { numerator = 15; denominator = 2; }
            else if (betName == BetName::Horn) { numerator = 30; denominator = 4; }
            else { numerator = 26; denominator = 5; }
            const auto cap = static_cast<std::uint64_t>(aggregate.maximumPotentialWin);
            const auto maxByWin = static_cast<Gen::Money>(
                (((cap + 1) * denominator) - 1) / numerator);
            limits.maximum = std::min(limits.maximum, maxByWin);
        }
    }
    limits.maximum -= limits.maximum % limits.increment;
    if (limits.minimum % limits.increment != 0)
        limits.minimum += limits.increment - (limits.minimum % limits.increment);
    return limits;
}

std::string
CrapsTable::diagPrefix(size_t idx, const CrapsBet& bet) const
{
    std::string diag("CrapsTable::");
    if (idx == 1)
    {
        diag += "addBet(): Unable to add ";
    } else if (idx == 2)
    {
        diag += "setContractAmount(): Unable to set contract amount for ";
    } else if (idx == 3)
    {
        diag += "setOddsAmount(): Unable to set odds amount for ";
    } else if (idx == 4)
    {
        diag += "removtBet(): Unable to remove bet for ";
    } else
    {
        assert(false && "diag bad index");
    }

    return diag + bet.diagBetId() + "; ";
}

//----------------------------------------------------------------

bool
CrapsTable::fifBettingClosed(const CrapsBet& bet, size_t idx, Gen::ErrorPass& ep) const
{
    // fault if betting is closed and sets ep error diag
    if (!bettingOpen_)
    {
        ep.setDescription(diagPrefix(1, bet) + "Betting is closed at "
                          "the moment - dice roll is underway.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::InvalidAccessError);
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
        ep.setDescription(diagPrefix(1, bet) + "Player " +
                          bet.player().getName()         +
                          " is not joined with this table.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::NotFoundError);
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifMaxPlayers(Gen::ErrorPass& ep) const
{
    if (players_.size() == MaxPlayers)
    {
        EP_SET(ep, En::OverflowError, Et::ProcessingError, Es::Indeterminate,
               "Maximum players already at table.");
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifHavePlayer(const Player& player, Gen::ErrorPass& ep) const
{
    if (havePlayer(&player))
    {
        EP_SET(ep, En::ExistsError, Et::ProcessingError, Es::Indeterminate,
               "Player already at table.");
        return true;
    }
    return false;
}

//----------------------------------------------------------------

bool
CrapsTable::fifHaveBet(const CrapsBet& bet, Gen::ErrorPass& ep) const
{
    // fault if already have bet and sets ep error diag
    if (haveBet(bet.betId()))
    {
        ep.setDescription(diagPrefix(1, bet) + "Player " +
                          bet.player().getName()         +
                          " has already made this bet.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::ExistsError);
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::fifBetPtrNull(const BetPtr       pBet,
                          const std::string& fName,
                          Gen::ErrorPass&    ep) const
{
    // fault if betPtr is null
    if (pBet == nullptr)
    {
        ep.setDescription("CrapsTable::" + fName + " pBet is nullptr.");
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::fifMissingBet(const CrapsBet& bet, size_t idx, Gen::ErrorPass& ep) const
{
    // fault if bet is not on the table and sets ep error diag
    if (!haveBet(bet.betId()))
    {
        ep.setDescription(diagPrefix(idx, bet) +
                          "This bet is not on the table.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::NotFoundError);
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::fifBetMustRemain(BetPtr pBet, Gen::ErrorPass& ep) const
{
    if (!isBetRemovable(pBet))
    {
        EP_SET(ep, En::UnsetErrorName, Et::ProcessingError, Es::Indeterminate,
               "Unable to remove bet. PassLine|Come bets with points "
               "must remain on table until a decision; " +
               pBet->diagBetId() + ".");
        ep.setErrorName("Betting Error");
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
        ep.setDescription(diagPrefix(idx, bet) + "Bet cannot be $0.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::InvalidArgumentError);
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
            ep.setDescription(diagPrefix(1, bet) +
                              "This bet is not allowed "
                              "during come out roll.");
            ep.setErrorName(Gen::ErrorPass::ErrorName::IllegalStateError);
            return true;
        }
        if (bet.pivot() != 0)
        {
            // Not a come out roll, but can't have a pivot already assigned
            ep.setDescription(diagPrefix(1, bet)                          +
                              "This bet cannot specify a point/pivot of " +
                              std::to_string(bet.pivot())                 +
                              ". Pivot must be 0.");
            ep.setErrorName(Gen::ErrorPass::ErrorName::IllegalStateError);
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
        ep.setDescription(diagPrefix(1, bet) + "This bet is not allowed "
                          "while there is already a point.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::IllegalStateError);
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
            ep.setDescription(diagPrefix(2, bet) +
                              "Cannot reduce contract amount for PassLine/"
                              "Come bets after point is established.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::NoPermissionError);
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
                ep.setDescription(diagPrefix(2, bet) +
                    "Cannot increase contract amount for DontPass/"
                    "DontCome bets after point is established.");
                ep.setErrorName(Gen::ErrorPass::ErrorName::NoPermissionError);
                return true;
            }
            else
            {
                // Reducing bet OK; but have to deal with odds
                if (bet.oddsAmount() > (amt * rules_.wagerLimit(bet.betName()).maximum))
                {
                    ep.setDescription(diagPrefix(2, bet) + 
                    "Cannot reduce contract bet due to large odds bet. First"
                    "reduce odds amount before reducing contract amount, "
                    "otherwise odds bet would exceed table limits.");
                    ep.setErrorName(Gen::ErrorPass::ErrorName::RangeError);
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
        ep.setErrorName(Gen::ErrorPass::ErrorName::RangeError);
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
    if (!withinMinMaxSideBets(bet.betName(), amt, ep))
    {
        ep.prepend(diagPrefix(idx, bet));
        ep.setErrorName(Gen::ErrorPass::ErrorName::RangeError);
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
        ep.setErrorName("Not a Multiple");
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
        ep.setDescription(diagPrefix(3, bet) + s);
        ep.setErrorName(Gen::ErrorPass::ErrorName::InvalidArgumentError);
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
        std::string s("Odds bet is only allowed after a "
            "point has been established for this bet.");
        ep.setDescription(diagPrefix(3, bet) + s);
        ep.setErrorName("No Point Established");
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::fifBadMinMaxForOdds(const CrapsBet& bet, Gen::Money amt,
                                Gen::ErrorPass& ep) const
{
    const auto limits = getBetAmountLimits(
        bet.betName(), bet.pivot(), true, bet.contractAmount());

    // Zero remains valid as the way to remove an existing odds wager.
    if (amt != 0 && (amt % limits.increment) != 0)
    {
        ep.setDescription(diagPrefix(3, bet) +
            "Odds wager must be a multiple of " +
            std::to_string(limits.increment) + ".");
        ep.setErrorName("Not a Multiple");
        return true;
    }

    if (amt > limits.maximum)
    {
        ep.setDescription(diagPrefix(3, bet) +
            Gen::MoneyUtils::toString(amt) +
            " exceeds the maximum odds amount of " +
            Gen::MoneyUtils::toString(limits.maximum) +
            " for this contract and table rules.");
        ep.setErrorName(Gen::ErrorPass::ErrorName::RangeError);
        return true;
    }
    return false;
}
        
//----------------------------------------------------------------

bool
CrapsTable::withinMinMaxLineBets(BetName betName,
                                 Gen::Money amt,
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
        const auto& limit = rules_.wagerLimit(betName);
        if (amt < limit.minimum || amt > limit.maximum)
        {
            ep.setDescription(diagLimits(amt) +
                "(min:" + std::to_string(limit.minimum) +
                ",max:" + std::to_string(limit.maximum) + ").");
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
    if (betName != BetName::Field && betName != BetName::CandE &&
        betName != BetName::Horn && betName != BetName::World)
        return true;

    const auto& limit = rules_.wagerLimit(betName);
    if (amt < limit.minimum || amt > limit.maximum)
    {
        ep.setDescription(diagLimits(amt) +
            "(min:" + std::to_string(limit.minimum) +
            ",max:" + std::to_string(limit.maximum) + ").");
        return false;
    }

    unsigned components = 0;
    switch (betName)
    {
        case BetName::CandE: components = 2; break;
        case BetName::Horn:  components = 4; break;
        case BetName::World: components = 5; break;
        default: break;
    }
    if (components == 0) return true;

    const auto& aggregate = rules_.aggregateLimit(betName);
    if (amt % components != 0)
    {
        ep.setDescription("This bet must be a multiple of " +
                          std::to_string(components) + ". " +
                          Gen::MoneyUtils::toString(amt) +
                          " cannot be divided evenly among its " +
                          std::to_string(components) + " components.");
        return false;
    }
    const Gen::Money component = amt / components;
    if (aggregate.maximumComponentWager > 0 &&
        component > aggregate.maximumComponentWager)
    {
        ep.setDescription("Aggregate component wager " +
            Gen::MoneyUtils::toString(component) + " exceeds component maximum " +
            Gen::MoneyUtils::toString(aggregate.maximumComponentWager) + ".");
        return false;
    }

    if (aggregate.basis == TableRules::LimitBasis::PotentialWin &&
        aggregate.maximumPotentialWin > 0)
    {
        std::uint64_t numerator = 0;
        std::uint64_t denominator = 1;
        if (betName == BetName::CandE) { numerator = 15; denominator = 2; }
        else if (betName == BetName::Horn) { numerator = 30; denominator = 4; }
        else { numerator = 26; denominator = 5; }
        const std::uint64_t potentialWin =
            (static_cast<std::uint64_t>(amt) * numerator) / denominator;
        if (potentialWin > aggregate.maximumPotentialWin)
        {
            ep.setDescription("Aggregate bet potential win exceeds table maximum " +
                Gen::MoneyUtils::toString(aggregate.maximumPotentialWin) + ".");
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------

bool
CrapsTable::goodMultiplesSideBets(BetName betName,
                          Gen::Money amt,
                          Gen::ErrorPass& ep) const
{
    const auto increment = rules_.wagerLimit(betName).increment;
    if (increment > 1 && (amt % increment != 0))
    {
        ep.setDescription("This bet must be a multiple of " +
                          std::to_string(increment) + ". " +
                          Gen::MoneyUtils::toString(amt) +
                          " is not a multiple of " +
                          std::to_string(increment) + ".");
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

