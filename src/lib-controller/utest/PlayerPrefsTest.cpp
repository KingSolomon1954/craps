//----------------------------------------------------------------
//
// File: PlayerPrefsTest.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerPrefs.h>
#include <craps/EnumBetName.h>
#include <doctest/doctest.h>
#include <yaml-cpp/yaml.h>

using Ctrl::LastFill;
using Ctrl::PlayerPrefs;

namespace {

LastFill::LastFillEntry
makeEntry(BetName name, size_t pivot, bool isOddsBet, unsigned amount)
{
    LastFill::LastFillEntry e;
    e.betName   = name;
    e.pivot     = pivot;
    e.isOddsBet = isOddsBet;
    e.amount    = amount;
    return e;
}

} // namespace

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:empty toYAML")
{
    PlayerPrefs prefs;
    const YAML::Node n = prefs.toYAML();
    REQUIRE(n.IsMap());
    CHECK(n["LastFills"].IsSequence());
    CHECK(n["LastFills"].size() == 0);
}

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:YAML round trip")
{
    PlayerPrefs src;
    src.lastFill().setLastFill(makeEntry(BetName::PassLine, 0, false, 50));
    src.lastFill().setLastFill(makeEntry(BetName::Place,    6, false, 12));

    PlayerPrefs dst;
    dst.lastFill().setLastFill(makeEntry(BetName::Field, 0, false, 5));
    dst.fromYAML(src.toYAML());

    const auto& fills = dst.lastFill().getLastFills();
    REQUIRE(fills.size() == 2);
    CHECK((fills[0].betName == BetName::PassLine));
    CHECK(fills[0].amount == 50);
    CHECK((fills[1].betName == BetName::Place));
    CHECK(fills[1].pivot  == 6);
}

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:fromYAML missing or wrong shape gives empty")
{
    PlayerPrefs prefs;
    prefs.lastFill().setLastFill(makeEntry(BetName::Field, 0, false, 5));

    SUBCASE("null node")
    {
        prefs.fromYAML(YAML::Node());
        CHECK(prefs.lastFill().getLastFills().empty());
    }

    SUBCASE("section missing from parent")
    {
        const YAML::Node root = YAML::Load("other: 1");
        prefs.fromYAML(root["Prefs"]);
        CHECK(prefs.lastFill().getLastFills().empty());
    }

    SUBCASE("scalar instead of map")
    {
        prefs.fromYAML(YAML::Load("hello"));
        CHECK(prefs.lastFill().getLastFills().empty());
    }

    SUBCASE("sequence instead of map")
    {
        prefs.fromYAML(YAML::Load("[1, 2]"));
        CHECK(prefs.lastFill().getLastFills().empty());
    }

    SUBCASE("map without LastFills")
    {
        prefs.fromYAML(YAML::Load("Something: 1"));
        CHECK(prefs.lastFill().getLastFills().empty());
    }
}

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:instances are independent")
{
    PlayerPrefs a;
    PlayerPrefs b;
    a.lastFill().setLastFill(makeEntry(BetName::Horn, 0, false, 7));

    CHECK(a.lastFill().getLastFills().size() == 1);
    CHECK(b.lastFill().getLastFills().empty());
}

//----------------------------------------------------------------
