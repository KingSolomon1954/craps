//----------------------------------------------------------------
//
// File: QuickBetTest.cpp
//
//----------------------------------------------------------------

#include <controller/QuickBet.h>
#include <craps/EnumBetName.h>
#include <doctest/doctest.h>
#include <string>
#include <yaml-cpp/yaml.h>

using Ctrl::QuickBet;

namespace {

QuickBet::QuickBetEntry
makeEntry(BetName name, size_t pivot, bool isOddsBet, unsigned amount)
{
    QuickBet::QuickBetEntry e;
    e.betName   = name;
    e.pivot     = pivot;
    e.isOddsBet = isOddsBet;
    e.amount    = amount;
    return e;
}

} // namespace

//----------------------------------------------------------------
// Plain data: none of these touch globals or publish events, so they
// run without a PlayerManager or EventManager subscribers.

TEST_CASE("QuickBet:set, replace, delete by entry and by index")
{
    QuickBet qb;
    qb.setQuickBet(makeEntry(BetName::PassLine, 0, false, 10));
    qb.setQuickBet(makeEntry(BetName::Field,    0, false, 5));
    qb.setQuickBet(makeEntry(BetName::Place,    6, false, 12));
    REQUIRE(qb.getQuickBets().size() == 3);

    // Same key replaces the amount and keeps its position
    qb.setQuickBet(makeEntry(BetName::Field, 0, false, 25));
    REQUIRE(qb.getQuickBets().size() == 3);
    CHECK((qb.getQuickBets()[1].betName == BetName::Field));
    CHECK(qb.getQuickBets()[1].amount == 25u);

    qb.deleteQuickBet(makeEntry(BetName::Field, 0, false, 0));
    REQUIRE(qb.getQuickBets().size() == 2);
    CHECK((qb.getQuickBets()[1].betName == BetName::Place));  // shifted up

    qb.deleteQuickBet(size_t(0));
    REQUIRE(qb.getQuickBets().size() == 1);
    CHECK((qb.getQuickBets()[0].betName == BetName::Place));

    qb.deleteQuickBet(size_t(7));                              // out of range
    CHECK(qb.getQuickBets().size() == 1);

    qb.clear();
    CHECK(qb.getQuickBets().empty());
}

//----------------------------------------------------------------

TEST_CASE("QuickBet:YAML round trip keeps order")
{
    QuickBet src;
    src.setQuickBet(makeEntry(BetName::Place,    8, false, 12));
    src.setQuickBet(makeEntry(BetName::PassLine, 0, true,  30));
    src.setQuickBet(makeEntry(BetName::Horn,     0, false, 4));

    QuickBet dst;
    dst.setQuickBet(makeEntry(BetName::Field, 0, false, 1));   // replaced
    dst.fromYAML(src.toYAML());

    const auto& v = dst.getQuickBets();
    REQUIRE(v.size() == 3);
    CHECK((v[0].betName == BetName::Place));
    CHECK(v[0].pivot == 8);
    CHECK((v[1].betName == BetName::PassLine));
    CHECK(v[1].isOddsBet == true);
    CHECK((v[2].betName == BetName::Horn));
}

//----------------------------------------------------------------

TEST_CASE("QuickBet:fromYAML is forgiving")
{
    QuickBet qb;
    qb.setQuickBet(makeEntry(BetName::Field, 0, false, 5));

    SUBCASE("null node")
    {
        qb.fromYAML(YAML::Node());
        CHECK(qb.getQuickBets().empty());
    }

    SUBCASE("not a sequence")
    {
        qb.fromYAML(YAML::Load("hello"));
        CHECK(qb.getQuickBets().empty());
    }

    SUBCASE("bad entries skipped, order of the good ones kept")
    {
        qb.fromYAML(YAML::Load(R"(
- betName: Horn
  pivot: 0
  isOddsBet: false
  amount: 4
- betName: Bogus
  pivot: 0
  isOddsBet: false
  amount: 1
- betName: Come
  pivot: 0
  isOddsBet: false
  amount: -3
- betName: Field
  pivot: 0
  isOddsBet: false
  amount: 9
)"));
        const auto& v = qb.getQuickBets();
        REQUIRE(v.size() == 2);
        CHECK((v[0].betName == BetName::Horn));
        CHECK((v[1].betName == BetName::Field));
    }
}

//----------------------------------------------------------------
