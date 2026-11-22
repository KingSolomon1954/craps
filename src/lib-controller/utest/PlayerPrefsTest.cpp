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
    for (const char* key : {"LastFills", "QuickBets"})
    {
        CHECK(n[key].IsSequence());
        CHECK(n[key].size() == 0);
    }
}

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:YAML round trip")
{
    PlayerPrefs src;
    src.lastFill().setLastFill(makeEntry(BetName::PassLine, 0, false, 50));
    src.lastFill().setLastFill(makeEntry(BetName::Place,    6, false, 12));

    Ctrl::BetPlan plan;
    plan.name = "Place Hi";
    plan.specs.push_back(Ctrl::BetSpec{BetName::Place, 8, false, 24});
    REQUIRE(src.quickBets().set(plan));

    PlayerPrefs dst;
    dst.lastFill().setLastFill(makeEntry(BetName::Field, 0, false, 5));
    dst.fromYAML(src.toYAML());

    const auto& fills = dst.lastFill().getLastFills();
    REQUIRE(fills.size() == 2);
    CHECK((fills[0].betName == BetName::PassLine));
    CHECK(fills[0].amount == 50);
    CHECK((fills[1].betName == BetName::Place));
    CHECK(fills[1].pivot  == 6);

    REQUIRE(dst.quickBets().plans().size() == 1);
    CHECK(dst.quickBets().plans()[0].name == "Place Hi");
    CHECK(dst.quickBets().plans()[0].specs[0].amount == 24u);
}

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:fromYAML missing or wrong shape gives empty")
{
    PlayerPrefs prefs;
    prefs.lastFill().setLastFill(makeEntry(BetName::Field, 0, false, 5));
    Ctrl::BetPlan plan;
    plan.name = "P";
    plan.specs.push_back(Ctrl::BetSpec{BetName::Field, 0, false, 5});
    prefs.quickBets().set(plan);

    SUBCASE("null node")
    {
        prefs.fromYAML(YAML::Node());
        CHECK(prefs.lastFill().getLastFills().empty());
        CHECK(prefs.quickBets().plans().empty());
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

    SUBCASE("map without any of the lists")
    {
        prefs.fromYAML(YAML::Load("Something: 1"));
        CHECK(prefs.lastFill().getLastFills().empty());
    }

    SUBCASE("a bad list is emptied")
    {
        prefs.fromYAML(YAML::Load("LastFills: not a list"));
        CHECK(prefs.lastFill().getLastFills().empty());
        CHECK(prefs.quickBets().plans().empty());          // key missing
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
    CHECK(b.quickBets().plans().empty());
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Player files written before AutoFill and the old QuickBet were
// removed may still carry an AutoFills section, and QuickBets entries
// in the old one-bet form. They must load without trouble. AutoFills
// is not written back, and old-form QuickBets entries are dropped.
//
TEST_CASE("PlayerPrefs:legacy AutoFills and old-form QuickBets are ignored")
{
    PlayerPrefs prefs;
    prefs.fromYAML(YAML::Load(R"(
LastFills:
  - betName: Come
    pivot: 0
    isOddsBet: false
    amount: 7
AutoFills:
  - betName: Come
    pivot: 0
    isOddsBet: false
    amount: 9
QuickBets:
  - betName: Horn
    pivot: 0
    isOddsBet: false
    amount: 4
)"));

    REQUIRE(prefs.lastFill().getLastFills().size() == 1);
    CHECK(prefs.lastFill().getLastFills()[0].amount == 7u);

    const YAML::Node out = prefs.toYAML();
    CHECK(!out["AutoFills"].IsDefined());
    CHECK(prefs.quickBets().plans().empty());
}

//----------------------------------------------------------------
