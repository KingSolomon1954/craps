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
    source.odds.wagerIncrement = 5;
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
    source.aggregateLimit(BetName::World).basis = TableRules::LimitBasis::PotentialWin;
    source.aggregateLimit(BetName::World).maximumPotentialWin = 1000;
    source.aggregateLimit(BetName::World).maximumComponentWager = 25;

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
    CHECK(restored.odds.wagerIncrement == 5);
    CHECK(restored.odds.maximumPotentialWin == 600);
    CHECK(restored.odds.payoutUnit == 5);
    CHECK(restored.fieldPayouts[2].numerator == 2);
    CHECK(restored.fieldPayouts[12].numerator == 3);
    CHECK(restored.buyCommission.basis == TableRules::CommissionBasis::OnWager);
    CHECK(restored.buyCommission.collection == TableRules::CommissionCollection::OnPlacement);
    CHECK(restored.buyCommission.minimum == 1);
    CHECK(restored.aggregateLimit(BetName::World).basis == TableRules::LimitBasis::PotentialWin);
    CHECK(restored.aggregateLimit(BetName::World).maximumPotentialWin == 1000);
    CHECK(restored.aggregateLimit(BetName::World).maximumComponentWager == 25);
    CHECK(restored.aggregateLimit(BetName::CandE).basis == TableRules::LimitBasis::AmountWagered);
    CHECK(restored.aggregateLimit(BetName::CandE).maximumPotentialWin == 0);
}


TEST_CASE("TableRules: reads legacy table-wide aggregate settings")
{
    TableRules defaults;
    YAML::Node yaml = defaults.toYAML();
    yaml.remove("aggregates");
    yaml["aggregateLimitBasis"] = "potentialWin";
    yaml["aggregateMaximumPotentialWin"] = 750;
    yaml["aggregateMaximumComponentWager"] = 20;

    TableRules restored;
    restored.fromYAML(yaml);
    for (const auto name : {BetName::CandE, BetName::Horn, BetName::World})
    {
        CHECK(restored.aggregateLimit(name).basis == TableRules::LimitBasis::PotentialWin);
        CHECK(restored.aggregateLimit(name).maximumPotentialWin == 750);
        CHECK(restored.aggregateLimit(name).maximumComponentWager == 20);
    }
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

    TableRules validRules;
    yaml = validRules.toYAML();
    yaml["buyCommission"]["collection"] = "onPlacement";
    yaml["buyCommission"]["basis"] = "onWin";
    CHECK_THROWS(rules.fromYAML(yaml));
}


TEST_CASE("TableRules: accepts both C&E aggregate YAML keys")
{
    YAML::Node yaml = YAML::Load(R"yaml(
aggregates:
  CandE:
    maximumComponentWager: 3
)yaml");
    TableRules rules;
    rules.fromYAML(yaml);
    CHECK(rules.aggregateLimit(BetName::CandE).maximumComponentWager == 3);

    yaml = YAML::Load(R"yaml(
aggregates:
  "C&E":
    maximumComponentWager: 7
)yaml");
    rules.fromYAML(yaml);
    CHECK(rules.aggregateLimit(BetName::CandE).maximumComponentWager == 7);
}
