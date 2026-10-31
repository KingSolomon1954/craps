//----------------------------------------------------------------
// File: TableRulesTest.cpp
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <craps/TableRules.h>

using namespace Craps;

TEST_CASE("TableRules: YAML round trip preserves configurable rules")
{
    TableRules source;
    source.wagerLimit(BetName::World) = {10, 500, 5};
    source.odds.style = TableRules::OddsLimitStyle::PointSchedule;
    source.odds.pointMultipliers[4] = 3;
    source.odds.pointMultipliers[5] = 4;
    source.odds.pointMultipliers[6] = 5;
    source.odds.maximumWager = 250;
    source.odds.maximumPotentialWin = 600;
    source.odds.rounding = TableRules::PayoutRounding::Down;
    source.odds.payoutUnit = 5;
    source.fieldPayouts[2] = {2, 1};
    source.fieldPayouts[12] = {3, 1};
    source.buyCommission.basisPoints = 500;
    source.buyCommission.basis = TableRules::CommissionBasis::OnWager;
    source.buyCommission.collection = TableRules::CommissionCollection::OnPlacement;
    source.buyCommission.minimum = 1;
    source.buyCommission.roundingUnit = 1;
    source.aggregateLimitBasis = TableRules::LimitBasis::PotentialWin;
    source.aggregateMaximumPotentialWin = 1000;
    source.enforceAggregateComponentLimits = true;

    const YAML::Node yaml = source.toYAML();
    TableRules restored;
    restored.fromYAML(yaml);

    CHECK(restored.wagerLimit(BetName::World).minimum == 10);
    CHECK(restored.wagerLimit(BetName::World).maximum == 500);
    CHECK(restored.wagerLimit(BetName::World).increment == 5);
    CHECK(restored.odds.style == TableRules::OddsLimitStyle::PointSchedule);
    CHECK(restored.odds.pointMultipliers[4] == 3);
    CHECK(restored.odds.pointMultipliers[5] == 4);
    CHECK(restored.odds.pointMultipliers[6] == 5);
    CHECK(restored.odds.maximumWager == 250);
    CHECK(restored.odds.maximumPotentialWin == 600);
    CHECK(restored.odds.payoutUnit == 5);
    CHECK(restored.fieldPayouts[2].numerator == 2);
    CHECK(restored.fieldPayouts[12].numerator == 3);
    CHECK(restored.buyCommission.basis == TableRules::CommissionBasis::OnWager);
    CHECK(restored.buyCommission.collection == TableRules::CommissionCollection::OnPlacement);
    CHECK(restored.buyCommission.minimum == 1);
    CHECK(restored.aggregateLimitBasis == TableRules::LimitBasis::PotentialWin);
    CHECK(restored.aggregateMaximumPotentialWin == 1000);
    CHECK(restored.enforceAggregateComponentLimits);
}

TEST_CASE("TableRules: rejects invalid configuration")
{
    TableRules rules;
    YAML::Node yaml = rules.toYAML();
    yaml["wagers"]["World"]["increment"] = 0;
    CHECK_THROWS(rules.fromYAML(yaml));

    yaml = rules.toYAML();
    yaml["odds"]["style"] = "unknownStyle";
    CHECK_THROWS(rules.fromYAML(yaml));

    yaml = rules.toYAML();
    yaml["fieldPayouts"]["12"]["denominator"] = 0;
    CHECK_THROWS(rules.fromYAML(yaml));
}
