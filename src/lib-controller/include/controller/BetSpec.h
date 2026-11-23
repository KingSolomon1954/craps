//----------------------------------------------------------------
//
// File: BetSpec.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <string>
#include <vector>

namespace Ctrl {

/// One bet to place: a spot and an amount
struct BetSpec
{
    BetName    betName = BetName::Invalid;
    unsigned   pivot   = 0;        ///< Point or number; 0 if the bet has none
    bool       isOdds  = false;    ///< Amount is for the odds on the bet
    Gen::Money amount  = 0;

    bool sameSpot(const BetSpec& o) const
    {
        return betName == o.betName && pivot == o.pivot && isOdds == o.isOdds;
    }
};

/// A named, ordered list of bet specs. A QuickBets entry is a plan.
struct BetPlan
{
    std::string          name;
    std::vector<BetSpec> specs;
};

/// Short words for one spec, e.g. "Place 6 $24" or "PassLine odds $50"
std::string describe(const BetSpec& spec);

/// The specs of a plan in one line, e.g. "Place 6 $24, Place 8 $24"
std::string describe(const BetPlan& plan);

/// Odds are limited to the bets that have a point from the start
bool isPlanOddsBet(BetName betName);

/*-----------------------------------------------------------*//**

@file BetSpec.h

@brief BetSpec and BetPlan, the common currency of bet automation

See the Bet Automation section of the design document. QuickBets
entries are plans. AutoOdds and buddy automation produce specs too, and
all of them are applied by BetExecutor.

*/

} // namespace Ctrl

//----------------------------------------------------------------
