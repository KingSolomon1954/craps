//----------------------------------------------------------------
//
// File: TableRules.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

/**
 * Rules governing wagers, odds, payouts, and commissions at a craps table.
 *
 * This is a value object owned by CrapsTable.  It deliberately contains no
 * table state (players, bets, point, or bankroll); those are evaluated by
 * CrapsTable against these rules.
 */
struct TableRules
{
    struct WagerLimit
    {
        Gen::Money minimum   = 0;
        Gen::Money maximum   = 0;
        Gen::Money increment = 1;
    };

    enum class OddsLimitStyle
    {
        FlatMultiplier,
        PointSchedule
    };

    enum class PayoutRounding
    {
        Exact,
        Down
    };

    enum class CommissionBasis
    {
        OnWager,
        OnWin
    };

    enum class CommissionCollection
    {
        OnPlacement,
        OnWinningDecision
    };

    enum class LimitBasis
    {
        AmountWagered,
        PotentialWin
    };

    struct OddsLimit
    {
        OddsLimitStyle style = OddsLimitStyle::FlatMultiplier;
        unsigned flatMultiplier = 5;
        // Indexed by dice total; entries 0, 1, 7, 11, and 12 are unused
        // unless a future rule explicitly gives them meaning.
        std::array<unsigned, 13> pointMultipliers{};
        Gen::Money maximumWager = 0;      // zero means no separate cap
        Gen::Money maximumPotentialWin = 0;
        PayoutRounding rounding = PayoutRounding::Down;
        Gen::Money payoutUnit = 1;
    };

    struct Payout
    {
        unsigned numerator   = 1;
        unsigned denominator = 1;
    };

    struct Commission
    {
        unsigned basisPoints = 500;       // 500 = 5%
        CommissionBasis basis = CommissionBasis::OnWin;
        CommissionCollection collection = CommissionCollection::OnWinningDecision;
        Gen::Money minimum = 0;
        Gen::Money roundingUnit = 1;
        PayoutRounding rounding = PayoutRounding::Down;
    };

    // One entry for every real BetName; Count and Invalid are not indexed.
    std::array<WagerLimit, EnumBetName::enumerators.size()> wagers{};

    OddsLimit odds{};
    PayoutRounding payoutRounding = PayoutRounding::Down;
    Gen::Money payoutUnit = 1;

    // Field profit odds for each dice total.  Other totals are losing totals.
    std::array<Payout, 13> fieldPayouts{};

    Commission buyCommission{};
    Commission layCommission{};

    // Aggregate wager constraints can apply to the whole bet and/or each
    // component.  A zero cap means no additional cap.
    LimitBasis aggregateLimitBasis = LimitBasis::AmountWagered;
    Gen::Money aggregateMaximumPotentialWin = 0;
    bool enforceAggregateComponentLimits = false;

    TableRules();

    const WagerLimit& wagerLimit(BetName name) const;
    WagerLimit& wagerLimit(BetName name);

    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    void validate() const;
};

} // namespace Craps

//----------------------------------------------------------------
