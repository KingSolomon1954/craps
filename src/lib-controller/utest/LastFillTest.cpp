//----------------------------------------------------------------
//
// File: LastFillTest.cpp
//
//----------------------------------------------------------------

#include <controller/LastFill.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <string>
#include <yaml-cpp/yaml.h>
#include <doctest/doctest.h>

using Ctrl::LastFill;

namespace {

LastFill::LastFillEntry
makeEntry(BetName name, size_t pivot, bool isOddsBet, long long cents)
{
    LastFill::LastFillEntry e;
    e.betName   = name;
    e.pivot     = pivot;
    e.isOddsBet = isOddsBet;
    e.amount    = cents;
    return e;
}

} // namespace

//----------------------------------------------------------------

TEST_CASE("LastFill:YAML round trip")
{
    LastFill src;
    src.setLastFill(makeEntry(BetName::PassLine, 0, false, 5000));
    src.setLastFill(makeEntry(BetName::PassLine, 0, true,  10000));
    src.setLastFill(makeEntry(BetName::Place,    6, false, 1200));

    // Destination starts with something that must be replaced.
    LastFill dst;
    dst.setLastFill(makeEntry(BetName::Field, 0, false, 100));

    dst.fromYAML(src.toYAML());

    const auto& fills = dst.getLastFills();
    REQUIRE(fills.size() == 3);

    CHECK((fills[0].betName == BetName::PassLine));
    CHECK(fills[0].pivot     == 0);
    CHECK(fills[0].isOddsBet == false);
    CHECK(fills[0].amount    == Gen::Money(5000));

    CHECK((fills[1].betName == BetName::PassLine));
    CHECK(fills[1].isOddsBet == true);
    CHECK(fills[1].amount    == Gen::Money(10000));

    CHECK((fills[2].betName == BetName::Place));
    CHECK(fills[2].pivot     == 6);
    CHECK(fills[2].amount    == Gen::Money(1200));
}

//----------------------------------------------------------------

TEST_CASE("LastFill:toYAML shape")
{
    LastFill lf;
    CHECK(lf.toYAML().IsSequence());      // empty is still a sequence
    CHECK(lf.toYAML().size() == 0);

    lf.setLastFill(makeEntry(BetName::Hardway, 8, false, 2500));
    const YAML::Node seq = lf.toYAML();
    REQUIRE(seq.size() == 1);
    CHECK(seq[0]["betName"].as<std::string>() == "Hardway");
    CHECK(seq[0]["pivot"].as<size_t>()         == 8);
    CHECK(seq[0]["isOddsBet"].as<bool>()       == false);
}

//----------------------------------------------------------------

TEST_CASE("LastFill:fromYAML from text")
{
    const YAML::Node n = YAML::Load(R"(
- betName: PassLine
  pivot: 0
  isOddsBet: false
  amount: 5000
- betName: Place
  pivot: 6
  isOddsBet: false
  amount: 1200
)");

    LastFill lf;
    lf.fromYAML(n);

    const auto& fills = lf.getLastFills();
    REQUIRE(fills.size() == 2);
    CHECK((fills[1].betName == BetName::Place));
    CHECK(fills[1].amount == Gen::Money(1200));
}

//----------------------------------------------------------------

TEST_CASE("LastFill:fromYAML missing or wrong shape gives empty")
{
    LastFill lf;
    lf.setLastFill(makeEntry(BetName::Field, 0, false, 100));

    SUBCASE("null node")
    {
        lf.fromYAML(YAML::Node());
        CHECK(lf.getLastFills().empty());
    }

    SUBCASE("section missing from parent")
    {
        const YAML::Node root = YAML::Load("other: 1");
        lf.fromYAML(root["LastFills"]);
        CHECK(lf.getLastFills().empty());
    }

    SUBCASE("scalar instead of sequence")
    {
        lf.fromYAML(YAML::Load("hello"));
        CHECK(lf.getLastFills().empty());
    }

    SUBCASE("map instead of sequence")
    {
        lf.fromYAML(YAML::Load("a: 1"));
        CHECK(lf.getLastFills().empty());
    }
}

//----------------------------------------------------------------

TEST_CASE("LastFill:fromYAML skips bad entries, keeps good ones")
{
    const YAML::Node n = YAML::Load(R"(
- betName: Bogus
  pivot: 0
  isOddsBet: false
  amount: 100
- betName: Field
  isOddsBet: false
  amount: 100
- betName: Field
  pivot: 0
  isOddsBet: false
  amount: -5
- betName: Field
  pivot: 0
  isOddsBet: false
  amount: {x: 1}
- betName: Horn
  pivot: 0
  isOddsBet: false
  amount: 700
- not a map
)");

    LastFill lf;
    CHECK_NOTHROW(lf.fromYAML(n));

    const auto& fills = lf.getLastFills();
    REQUIRE(fills.size() == 1);
    CHECK((fills[0].betName == BetName::Horn));
    CHECK(fills[0].amount == Gen::Money(700));
}

//----------------------------------------------------------------

TEST_CASE("LastFill:fromYAML duplicates collapse, later wins")
{
    const YAML::Node n = YAML::Load(R"(
- betName: Come
  pivot: 0
  isOddsBet: false
  amount: 100
- betName: Come
  pivot: 0
  isOddsBet: false
  amount: 300
)");

    LastFill lf;
    lf.fromYAML(n);

    const auto& fills = lf.getLastFills();
    REQUIRE(fills.size() == 1);
    CHECK(fills[0].amount == Gen::Money(300));
}

//----------------------------------------------------------------

TEST_CASE("LastFill:clear")
{
    LastFill lf;
    lf.setLastFill(makeEntry(BetName::Field, 0, false, 100));
    REQUIRE(lf.getLastFills().size() == 1);
    lf.clear();
    CHECK(lf.getLastFills().empty());
}

//----------------------------------------------------------------
