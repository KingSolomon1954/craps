//----------------------------------------------------------------
//
// File: AutoOddsTest.cpp
//
//----------------------------------------------------------------

#include <controller/AutoOdds.h>
#include <doctest/doctest.h>
#include <yaml-cpp/yaml.h>

using namespace Ctrl;

namespace {

Craps::BetAmountLimits
limits(Gen::Money maximum, Gen::Money increment = 1)
{
    Craps::BetAmountLimits l;
    l.maximum   = maximum;
    l.increment = increment;
    return l;
}

AutoOdds::Setting
mult(unsigned n)
{
    AutoOdds::Setting s;
    s.multiple = n;
    return s;
}

AutoOdds::Setting
amt(Gen::Money n)
{
    AutoOdds::Setting s;
    s.amount = n;
    return s;
}

AutoOdds::Candidate
cand(unsigned id, BetName name, unsigned pivot, Gen::Money contract,
     Gen::Money odds = 0, Craps::BetAmountLimits l = limits(1000))
{
    AutoOdds::Candidate c;
    c.betId    = id;
    c.betName  = name;
    c.pivot    = pivot;
    c.contract = contract;
    c.odds     = odds;
    c.limits   = l;
    return c;
}

} // namespace

//----------------------------------------------------------------

TEST_CASE("AutoOdds: oddsAmount multiplies, clamps and rounds down")
{
    CHECK(AutoOdds::oddsAmount(mult(3), 10, limits(1000)) == 30);
    CHECK(AutoOdds::oddsAmount(mult(0), 10, limits(1000)) == 0);
    CHECK(AutoOdds::oddsAmount(mult(3), 0,  limits(1000)) == 0);
    CHECK(AutoOdds::oddsAmount(mult(100), 10, limits(50)) == 50);
    CHECK(AutoOdds::oddsAmount(mult(3), 10, limits(0)) == 30);        // no maximum
    CHECK(AutoOdds::oddsAmount(mult(3), 5, limits(1000, 10)) == 10);  // 15 -> 10
    CHECK(AutoOdds::oddsAmount(mult(1), 5, limits(1000, 10)) == 0);   // below one
    CHECK(AutoOdds::oddsAmount(mult(2), 25, limits(40, 10)) == 40);   // clamp
    CHECK(AutoOdds::oddsAmount(mult(2), 25, limits(45, 10)) == 40);   // 45 -> 40

    // Exact amounts ignore the contract
    CHECK(AutoOdds::oddsAmount(amt(50), 10, limits(1000)) == 50);
    CHECK(AutoOdds::oddsAmount(amt(50), 0,  limits(1000)) == 50);
    CHECK(AutoOdds::oddsAmount(amt(50), 10, limits(30)) == 30);
    CHECK(AutoOdds::oddsAmount(amt(55), 10, limits(1000, 10)) == 50);
    CHECK(AutoOdds::oddsAmount(amt(5),  10, limits(1000, 10)) == 0);
    CHECK(AutoOdds::oddsAmount(AutoOdds::Setting{}, 10, limits(1000)) == 0);
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: parseSetting reads what a person types")
{
    AutoOdds::Setting s;
    CHECK(AutoOdds::parseSetting("3x", s).empty());
    CHECK(s == mult(3));
    CHECK(AutoOdds::parseSetting(" 5 X ", s).empty());
    CHECK(s == mult(5));
    CHECK(AutoOdds::parseSetting("50", s).empty());
    CHECK(s == amt(50));
    CHECK(AutoOdds::parseSetting("$75", s).empty());
    CHECK(s == amt(75));
    CHECK(AutoOdds::parseSetting("0", s).empty());
    CHECK_FALSE(s.isSet());
    CHECK(AutoOdds::parseSetting("0x", s).empty());
    CHECK_FALSE(s.isSet());

    CHECK_FALSE(AutoOdds::parseSetting("", s).empty());
    CHECK_FALSE(AutoOdds::parseSetting("x", s).empty());
    CHECK_FALSE(AutoOdds::parseSetting("$", s).empty());
    CHECK_FALSE(AutoOdds::parseSetting("abc", s).empty());
    CHECK_FALSE(AutoOdds::parseSetting("-5", s).empty());
    CHECK_FALSE(AutoOdds::parseSetting("101x", s).empty());
    CHECK_FALSE(AutoOdds::parseSetting("12345678901", s).empty());
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: an exact amount is placed as is")
{
    AutoOdds a;
    a.setAmount(BetName::Come, 10, 50);
    CHECK(a.summary(BetName::Come) == "10:$50");
    auto p = a.plan({cand(1, BetName::Come, 10, 10)});
    REQUIRE(p.specs.size() == 1);
    CHECK(p.specs[0].amount == 50);

    // Replacing a multiple with an amount, and back
    a.setMultiple(BetName::Come, 10, 2);
    CHECK(a.amount(BetName::Come, 10) == 0);
    CHECK(a.multiple(BetName::Come, 10) == 2);
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: setting and clearing multiples")
{
    AutoOdds a;
    CHECK_FALSE(a.enabled());
    CHECK_FALSE(a.anySet());

    CHECK(a.setMultiple(BetName::PassLine, 6, 5).empty());
    CHECK(a.multiple(BetName::PassLine, 6) == 5);
    CHECK(a.multiple(BetName::PassLine, 8) == 0);
    CHECK(a.anySet());

    CHECK_FALSE(a.setMultiple(BetName::Place, 6, 5).empty());
    CHECK_FALSE(a.setMultiple(BetName::PassLine, 7, 5).empty());
    CHECK_FALSE(a.setMultiple(BetName::PassLine, 6, 101).empty());
    CHECK_FALSE(a.set(BetName::PassLine, 6, {2, 50}).empty());  // not both
    CHECK(a.multiple(BetName::PassLine, 6) == 5);

    CHECK(a.setMultiple(BetName::PassLine, 6, 0).empty());
    CHECK_FALSE(a.anySet());

    CHECK(a.setMultiple(BetName::Come, 0, 2).empty());      // all points
    CHECK(a.multiple(BetName::Come, 4) == 2);
    CHECK(a.multiple(BetName::Come, 10) == 2);
    CHECK(a.summary(BetName::Come) == "4:2x 5:2x 6:2x 8:2x 9:2x 10:2x");
    CHECK(a.summary(BetName::DontCome) == "none");

    a.setEnabled(true);
    a.clearAll();
    CHECK_FALSE(a.anySet());
    CHECK(a.enabled());
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: 3-4-5 preset")
{
    AutoOdds a;
    a.setPreset345();
    for (BetName b : AutoOdds::oddsBetNames())
    {
        CHECK(a.multiple(b, 4)  == 3);
        CHECK(a.multiple(b, 10) == 3);
        CHECK(a.multiple(b, 5)  == 4);
        CHECK(a.multiple(b, 9)  == 4);
        CHECK(a.multiple(b, 6)  == 5);
        CHECK(a.multiple(b, 8)  == 5);
    }
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: plan places odds on eligible bets")
{
    AutoOdds a;
    a.setMultiple(BetName::PassLine, 6, 5);
    a.setMultiple(BetName::Come, 8, 4);

    auto p = a.plan({
        cand(1, BetName::PassLine, 6, 10),
        cand(2, BetName::Come, 8, 10),
        cand(3, BetName::Come, 5, 10),          // no multiple at 5
        cand(4, BetName::Place, 6, 12),         // not an odds bet
        cand(5, BetName::PassLine, 0, 10)});    // no point yet

    REQUIRE(p.specs.size() == 2);
    CHECK(p.specs[0].betName == BetName::PassLine);
    CHECK(p.specs[0].pivot   == 0);             // line bets use the point
    CHECK(p.specs[0].isOdds);
    CHECK(p.specs[0].amount  == 50);
    CHECK(p.specs[1].betName == BetName::Come);
    CHECK(p.specs[1].pivot   == 8);
    CHECK(p.specs[1].amount  == 40);
    CHECK(p.skipped.empty());
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: a handled bet is not tried again")
{
    AutoOdds a;
    a.setMultiple(BetName::PassLine, 6, 5);
    const std::vector<AutoOdds::Candidate> bets{cand(1, BetName::PassLine, 6, 10)};

    CHECK(a.plan(bets).specs.size() == 1);
    CHECK(a.plan(bets).specs.empty());          // player took odds down

    a.forgetHandled();
    CHECK(a.plan(bets).specs.size() == 1);
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: a bet that is gone is forgotten")
{
    AutoOdds a;
    a.setMultiple(BetName::Come, 6, 2);
    CHECK(a.plan({cand(7, BetName::Come, 6, 10)}).specs.size() == 1);
    CHECK(a.plan({}).specs.empty());            // bet 7 is gone
    CHECK(a.plan({cand(7, BetName::Come, 6, 10)}).specs.size() == 1);
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: odds already on the bet are left alone")
{
    AutoOdds a;
    a.setMultiple(BetName::PassLine, 6, 5);
    auto p = a.plan({cand(1, BetName::PassLine, 6, 10, 20)});
    CHECK(p.specs.empty());
    CHECK(p.skipped.empty());

    // And stay down if the player later removes them
    CHECK(a.plan({cand(1, BetName::PassLine, 6, 10, 0)}).specs.empty());
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: below one increment is skipped with a reason")
{
    AutoOdds a;
    a.setMultiple(BetName::PassLine, 6, 1);
    auto p = a.plan({cand(1, BetName::PassLine, 6, 5, 0, limits(1000, 10))});
    CHECK(p.specs.empty());
    REQUIRE(p.skipped.size() == 1);
    CHECK(p.skipped[0].find("increment") != std::string::npos);
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: clamps to the table maximum")
{
    AutoOdds a;
    a.setMultiple(BetName::DontPass, 4, 100);
    auto p = a.plan({cand(1, BetName::DontPass, 4, 10, 0, limits(60))});
    REQUIRE(p.specs.size() == 1);
    CHECK(p.specs[0].amount == 60);
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: YAML round trip")
{
    AutoOdds a;
    a.setEnabled(true);
    a.setPreset345();
    a.setMultiple(BetName::Come, 6, 2);
    a.setAmount(BetName::DontCome, 8, 75);

    AutoOdds b;
    b.fromYAML(a.toYAML());
    CHECK(b.enabled());
    for (BetName n : AutoOdds::oddsBetNames())
    {
        for (unsigned pt : AutoOdds::points())
        {
            CHECK(b.setting(n, pt) == a.setting(n, pt));
        }
    }
}

//----------------------------------------------------------------

TEST_CASE("AutoOdds: fromYAML is forgiving")
{
    AutoOdds a;
    a.setEnabled(true);
    a.setMultiple(BetName::Come, 6, 2);

    a.fromYAML(YAML::Node());
    CHECK_FALSE(a.enabled());
    CHECK_FALSE(a.anySet());

    a.fromYAML(YAML::Load("[1, 2]"));
    CHECK_FALSE(a.anySet());

    a.fromYAML(YAML::Load(
        "enabled: yes\n"
        "multiples:\n"
        "  PassLine: {6: 5, 7: 3, 8: lots, 9: 4x, 10: $60}\n"
        "  Place: {6: 2}\n"
        "  Bogus: {6: 2}\n"
        "  Come: 4\n"
        "  DontCome: {4: 2}\n"));
    CHECK(a.enabled());
    CHECK(a.multiple(BetName::PassLine, 6) == 5);
    CHECK(a.multiple(BetName::PassLine, 8) == 0);
    CHECK(a.multiple(BetName::PassLine, 9) == 4);
    CHECK(a.amount(BetName::PassLine, 10) == 60);
    CHECK(a.multiple(BetName::DontCome, 4) == 2);
    CHECK(a.multiple(BetName::Place, 6) == 0);
}

//----------------------------------------------------------------
