//----------------------------------------------------------------
//
// File: AutoFillTest.cpp
//
//----------------------------------------------------------------

#include <controller/AutoFill.h>
#include <craps/EnumBetName.h>
#include <doctest/doctest.h>
#include <string>
#include <yaml-cpp/yaml.h>

using Ctrl::AutoFill;

namespace {

AutoFill::AutoFillEntry
makeEntry(BetName name, size_t pivot, bool isOddsBet, unsigned amount)
{
    AutoFill::AutoFillEntry e;
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

TEST_CASE("AutoFill:set, replace, get, delete")
{
    AutoFill af;
    af.setAutoFill(makeEntry(BetName::Place, 6, false, 12));
    af.setAutoFill(makeEntry(BetName::Place, 8, false, 12));
    af.setAutoFill(makeEntry(BetName::PassLine, 0, true, 30));
    REQUIRE(af.getAutoFills().size() == 3);

    // Same betName/pivot/isOddsBet replaces the amount only
    af.setAutoFill(makeEntry(BetName::Place, 6, false, 18));
    REQUIRE(af.getAutoFills().size() == 3);

    // isOddsBet is part of the key
    af.setAutoFill(makeEntry(BetName::PassLine, 0, false, 5));
    REQUIRE(af.getAutoFills().size() == 4);

    auto probe = makeEntry(BetName::Place, 6, false, 0);
    CHECK((af.getAutoFill(probe) == Gen::ReturnCode::Success));
    CHECK(probe.amount == 18u);

    auto missing = makeEntry(BetName::Place, 9, false, 0);
    CHECK((af.getAutoFill(missing) == Gen::ReturnCode::Fail));

    af.deleteAutoFill(makeEntry(BetName::Place, 6, false, 0));
    CHECK(af.getAutoFills().size() == 3);
    af.clear();
    CHECK(af.getAutoFills().empty());
}

//----------------------------------------------------------------

TEST_CASE("AutoFill:zero is a valid amount")
{
    AutoFill af;
    af.setAutoFill(makeEntry(BetName::Field, 0, false, 0));
    REQUIRE(af.getAutoFills().size() == 1);
    CHECK(af.getAutoFills()[0].amount == 0u);
}

//----------------------------------------------------------------

TEST_CASE("AutoFill:YAML round trip keeps order")
{
    AutoFill src;
    src.setAutoFill(makeEntry(BetName::Place,    6, false, 12));
    src.setAutoFill(makeEntry(BetName::PassLine, 0, true,  30));
    src.setAutoFill(makeEntry(BetName::Field,    0, false, 5));

    AutoFill dst;
    dst.setAutoFill(makeEntry(BetName::Horn, 0, false, 1));   // replaced
    dst.fromYAML(src.toYAML());

    const auto& v = dst.getAutoFills();
    REQUIRE(v.size() == 3);
    CHECK((v[0].betName == BetName::Place));
    CHECK(v[0].pivot == 6);
    CHECK((v[1].betName == BetName::PassLine));
    CHECK(v[1].isOddsBet == true);
    CHECK((v[2].betName == BetName::Field));
    CHECK(v[2].amount == 5u);
}

//----------------------------------------------------------------

TEST_CASE("AutoFill:fromYAML is forgiving")
{
    AutoFill af;
    af.setAutoFill(makeEntry(BetName::Field, 0, false, 5));

    SUBCASE("null node")
    {
        af.fromYAML(YAML::Node());
        CHECK(af.getAutoFills().empty());
    }

    SUBCASE("not a sequence")
    {
        af.fromYAML(YAML::Load("a: 1"));
        CHECK(af.getAutoFills().empty());
    }

    SUBCASE("bad entries skipped, good ones kept, duplicates collapse")
    {
        af.fromYAML(YAML::Load(R"(
- betName: Bogus
  pivot: 0
  isOddsBet: false
  amount: 1
- betName: Horn
  isOddsBet: false
  amount: 1
- betName: Come
  pivot: 0
  isOddsBet: false
  amount: 10
- betName: Come
  pivot: 0
  isOddsBet: false
  amount: 20
- not a map
)"));
        const auto& v = af.getAutoFills();
        REQUIRE(v.size() == 1);
        CHECK((v[0].betName == BetName::Come));
        CHECK(v[0].amount == 20u);
    }
}

//----------------------------------------------------------------
