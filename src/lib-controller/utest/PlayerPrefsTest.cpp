//----------------------------------------------------------------
//
// File: PlayerPrefsTest.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerPrefs.h>
#include <craps/EnumBetName.h>
#include <doctest/doctest.h>
#include <yaml-cpp/yaml.h>

using Ctrl::AutoFill;
using Ctrl::LastFill;
using Ctrl::PlayerPrefs;
using Ctrl::QuickBet;

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
    for (const char* key : {"LastFills", "AutoFills", "QuickBets"})
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

    src.autoFill().setAutoFill(AutoFill::AutoFillEntry{BetName::Come, 0, false, 20});
    src.quickBet().setQuickBet(QuickBet::QuickBetEntry{BetName::Horn, 0, false, 4});
    src.quickBet().setQuickBet(QuickBet::QuickBetEntry{BetName::Field, 0, false, 9});

    PlayerPrefs dst;
    dst.lastFill().setLastFill(makeEntry(BetName::Field, 0, false, 5));
    dst.autoFill().setAutoFill(AutoFill::AutoFillEntry{BetName::Hardway, 8, false, 1});
    dst.fromYAML(src.toYAML());

    const auto& fills = dst.lastFill().getLastFills();
    REQUIRE(fills.size() == 2);
    CHECK((fills[0].betName == BetName::PassLine));
    CHECK(fills[0].amount == 50);
    CHECK((fills[1].betName == BetName::Place));
    CHECK(fills[1].pivot  == 6);

    const auto& af = dst.autoFill().getAutoFills();
    REQUIRE(af.size() == 1);
    CHECK((af[0].betName == BetName::Come));
    CHECK(af[0].amount == 20u);

    const auto& qb = dst.quickBet().getQuickBets();
    REQUIRE(qb.size() == 2);
    CHECK((qb[0].betName == BetName::Horn));        // order kept
    CHECK((qb[1].betName == BetName::Field));
}

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:fromYAML missing or wrong shape gives empty")
{
    PlayerPrefs prefs;
    prefs.lastFill().setLastFill(makeEntry(BetName::Field, 0, false, 5));
    prefs.autoFill().setAutoFill(AutoFill::AutoFillEntry{BetName::Field, 0, false, 5});
    prefs.quickBet().setQuickBet(QuickBet::QuickBetEntry{BetName::Field, 0, false, 5});

    SUBCASE("null node")
    {
        prefs.fromYAML(YAML::Node());
        CHECK(prefs.lastFill().getLastFills().empty());
        CHECK(prefs.autoFill().getAutoFills().empty());
        CHECK(prefs.quickBet().getQuickBets().empty());
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
        CHECK(prefs.autoFill().getAutoFills().empty());
        CHECK(prefs.quickBet().getQuickBets().empty());
    }

    SUBCASE("one bad list only empties that list")
    {
        prefs.fromYAML(YAML::Load(R"(
LastFills: not a list
AutoFills:
  - betName: Come
    pivot: 0
    isOddsBet: false
    amount: 7
)"));
        CHECK(prefs.lastFill().getLastFills().empty());
        REQUIRE(prefs.autoFill().getAutoFills().size() == 1);
        CHECK(prefs.autoFill().getAutoFills()[0].amount == 7u);
        CHECK(prefs.quickBet().getQuickBets().empty());   // key missing
    }
}

//----------------------------------------------------------------

TEST_CASE("PlayerPrefs:instances are independent")
{
    PlayerPrefs a;
    PlayerPrefs b;
    a.lastFill().setLastFill(makeEntry(BetName::Horn, 0, false, 7));
    a.autoFill().setAutoFill(AutoFill::AutoFillEntry{BetName::Horn, 0, false, 7});
    a.quickBet().setQuickBet(QuickBet::QuickBetEntry{BetName::Horn, 0, false, 7});

    CHECK(a.lastFill().getLastFills().size() == 1);
    CHECK(b.lastFill().getLastFills().empty());
    CHECK(b.autoFill().getAutoFills().empty());
    CHECK(b.quickBet().getQuickBets().empty());
}

//----------------------------------------------------------------
