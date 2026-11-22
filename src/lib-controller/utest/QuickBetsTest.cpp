//----------------------------------------------------------------
//
// File: QuickBetsTest.cpp
//
//----------------------------------------------------------------

#include <controller/QuickBets.h>
#include <doctest/doctest.h>
#include <yaml-cpp/yaml.h>

using namespace Ctrl;

namespace {

BetSpec
spec(BetName name, unsigned pivot, Gen::Money amount, bool odds = false)
{
    BetSpec s;
    s.betName = name;
    s.pivot   = pivot;
    s.amount  = amount;
    s.isOdds  = odds;
    return s;
}

BetPlan
plan(const std::string& name, std::vector<BetSpec> specs)
{
    BetPlan p;
    p.name  = name;
    p.specs = std::move(specs);
    return p;
}

} // namespace

//----------------------------------------------------------------

TEST_CASE("QuickBets:set adds, and replaces by name")
{
    QuickBets qb;
    CHECK(qb.set(plan("Hi", {spec(BetName::Place, 8, 24)})));
    CHECK(qb.set(plan("Lo", {spec(BetName::Place, 6, 12)})));
    REQUIRE(qb.plans().size() == 2);

    CHECK(qb.set(plan("Hi", {spec(BetName::Place, 9, 20)})));   // replaces
    REQUIRE(qb.plans().size() == 2);
    CHECK(qb.plans()[0].name == "Hi");                           // same place
    CHECK(qb.plans()[0].specs[0].pivot == 9);

    REQUIRE(qb.find("Lo") != nullptr);
    CHECK(qb.find("Nope") == nullptr);
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:set refuses what cannot be used")
{
    QuickBets qb;
    CHECK_FALSE(qb.set(plan("", {spec(BetName::Field, 0, 5)})));           // no name
    CHECK_FALSE(qb.set(plan("Empty", {})));                                // no bets
    CHECK_FALSE(qb.set(plan("Zero", {spec(BetName::Field, 0, 0)})));       // only unusable
    CHECK_FALSE(qb.set(plan("Odds", {spec(BetName::Come, 6, 10, true)}))); // Come odds
    CHECK(qb.plans().empty());
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:limits")
{
    QuickBets qb;
    for (std::size_t i = 0; i < QuickBets::MaxEntries; ++i)
    {
        CHECK(qb.set(plan("E" + std::to_string(i), {spec(BetName::Field, 0, 5)})));
    }
    CHECK_FALSE(qb.set(plan("One too many", {spec(BetName::Field, 0, 5)})));
    CHECK(qb.set(plan("E0", {spec(BetName::Field, 0, 10)})));   // replacing is fine

    std::vector<BetSpec> many;
    for (unsigned i = 0; i < 30; ++i)
    {
        many.push_back(spec(BetName::Place, 4 + i, 5));         // Distinct spots
    }
    const BetPlan tidy = QuickBets::tidy(plan("Many", many));
    CHECK(tidy.specs.size() == QuickBets::MaxSpecsPerEntry);
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:tidy")
{
    SUBCASE("contract bets come before odds, otherwise order is kept")
    {
        const BetPlan t = QuickBets::tidy(plan("T", {
            spec(BetName::PassLine, 0, 30, true),
            spec(BetName::Place, 6, 12),
            spec(BetName::PassLine, 0, 10),
            spec(BetName::Field, 0, 5)}));
        REQUIRE(t.specs.size() == 4);
        CHECK((t.specs[0].betName == BetName::Place));
        CHECK((t.specs[1].betName == BetName::PassLine));
        CHECK_FALSE(t.specs[1].isOdds);
        CHECK((t.specs[2].betName == BetName::Field));
        CHECK(t.specs[3].isOdds);
    }

    SUBCASE("a later bet for the same spot replaces the earlier")
    {
        const BetPlan t = QuickBets::tidy(plan("T", {
            spec(BetName::Place, 6, 12),
            spec(BetName::Field, 0, 5),
            spec(BetName::Place, 6, 18)}));
        REQUIRE(t.specs.size() == 2);
        CHECK((t.specs[0].betName == BetName::Place));
        CHECK(t.specs[0].amount == 18u);
    }

    SUBCASE("line odds are allowed for PassLine and DontPass only")
    {
        const BetPlan t = QuickBets::tidy(plan("T", {
            spec(BetName::PassLine, 0, 10),
            spec(BetName::PassLine, 0, 30, true),
            spec(BetName::DontPass, 0, 10),
            spec(BetName::DontPass, 0, 20, true),
            spec(BetName::DontCome, 6, 20, true)}));
        CHECK(t.specs.size() == 4);
    }
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:YAML round trip")
{
    QuickBets src;
    src.set(plan("Place Hi", {spec(BetName::Place, 8, 24),
                              spec(BetName::Place, 9, 20)}));
    src.set(plan("Line", {spec(BetName::PassLine, 0, 10),
                          spec(BetName::PassLine, 0, 30, true)}));

    QuickBets dst;
    dst.set(plan("Old", {spec(BetName::Field, 0, 5)}));
    dst.fromYAML(src.toYAML());

    REQUIRE(dst.plans().size() == 2);                  // Old was replaced
    CHECK(dst.plans()[0].name == "Place Hi");          // Order kept
    CHECK(dst.plans()[1].name == "Line");
    REQUIRE(dst.plans()[1].specs.size() == 2);
    CHECK_FALSE(dst.plans()[1].specs[0].isOdds);
    CHECK(dst.plans()[1].specs[1].isOdds);
    CHECK(dst.plans()[1].specs[1].amount == 30u);
    CHECK(dst.plans()[0].specs[1].pivot == 9u);
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:fromYAML is forgiving")
{
    QuickBets qb;
    qb.set(plan("Old", {spec(BetName::Field, 0, 5)}));

    SUBCASE("null or missing node gives empty")
    {
        qb.fromYAML(YAML::Node());
        CHECK(qb.plans().empty());
    }

    SUBCASE("not a sequence gives empty")
    {
        qb.fromYAML(YAML::Load("hello"));
        CHECK(qb.plans().empty());
    }

    SUBCASE("pivot and isOdds may be left out")
    {
        qb.fromYAML(YAML::Load(R"(
- name: Line
  bets:
    - { betName: PassLine, amount: 10 }
    - { betName: PassLine, isOdds: true, amount: 30 }
)"));
        REQUIRE(qb.plans().size() == 1);
        REQUIRE(qb.plans()[0].specs.size() == 2);
        CHECK(qb.plans()[0].specs[0].pivot == 0u);
        CHECK(qb.plans()[0].specs[1].isOdds);
    }

    SUBCASE("bad entries are skipped, good ones kept")
    {
        qb.fromYAML(YAML::Load(R"(
- name: Good
  bets:
    - { betName: Field, amount: 5 }
- name: No bets
- bets:
    - { betName: Field, amount: 5 }
- name: Bad amount
  bets:
    - { betName: Field, amount: lots }
- name: Bad bet name
  bets:
    - { betName: Nonsense, amount: 5 }
- just a string
- name: Also good
  bets:
    - { betName: Place, pivot: 6, amount: 12 }
)"));
        REQUIRE(qb.plans().size() == 2);
        CHECK(qb.plans()[0].name == "Good");
        CHECK(qb.plans()[1].name == "Also good");
    }
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:remove and clear")
{
    QuickBets qb;
    qb.set(plan("A", {spec(BetName::Field, 0, 5)}));
    qb.set(plan("B", {spec(BetName::Field, 0, 5)}));
    CHECK_FALSE(qb.remove(5));
    CHECK(qb.remove(0));
    REQUIRE(qb.plans().size() == 1);
    CHECK(qb.plans()[0].name == "B");
    qb.clear();
    CHECK(qb.plans().empty());
}

//----------------------------------------------------------------

TEST_CASE("BetSpec:describe")
{
    CHECK(describe(spec(BetName::Place, 6, 24))              == "Place 6 $24");
    CHECK(describe(spec(BetName::Field, 0, 5))               == "Field $5");
    CHECK(describe(spec(BetName::PassLine, 0, 50, true))     == "PassLine odds $50");
    CHECK(describe(plan("P", {spec(BetName::Field, 0, 5),
                              spec(BetName::Place, 8, 12)})) == "Field $5, Place 8 $12");
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:checkName and checkSpec")
{
    CHECK(QuickBets::checkName("Place Hi").empty());
    CHECK_FALSE(QuickBets::checkName("").empty());
    CHECK_FALSE(QuickBets::checkName(std::string(QuickBets::MaxNameLength + 1, 'x')).empty());
    CHECK(QuickBets::checkName(std::string(QuickBets::MaxNameLength, 'x')).empty());

    CHECK(QuickBets::checkSpec(spec(BetName::Place, 6, 24)).empty());
    CHECK(QuickBets::checkSpec(spec(BetName::PassLine, 0, 30, true)).empty());
    CHECK_FALSE(QuickBets::checkSpec(spec(BetName::Place, 6, 0)).empty());
    CHECK_FALSE(QuickBets::checkSpec(spec(BetName::Invalid, 0, 5)).empty());
    CHECK_FALSE(QuickBets::checkSpec(spec(BetName::DontCome, 5, 30, true)).empty());
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:create")
{
    QuickBets qb;
    CHECK(qb.create("Place Hi", spec(BetName::Place, 8, 24)).empty());
    REQUIRE(qb.plans().size() == 1);
    CHECK(qb.plans()[0].specs.size() == 1);

    SUBCASE("a name already used is refused, ignoring case")
    {
        CHECK_FALSE(qb.create("place hi", spec(BetName::Place, 9, 20)).empty());
        CHECK(qb.plans().size() == 1);
    }

    SUBCASE("a bad name or bet is refused")
    {
        CHECK_FALSE(qb.create("", spec(BetName::Place, 9, 20)).empty());
        CHECK_FALSE(qb.create("Odds", spec(BetName::Come, 6, 20, true)).empty());
        CHECK(qb.plans().size() == 1);
    }

    SUBCASE("refused when the list is full")
    {
        for (std::size_t i = 1; i < QuickBets::MaxEntries; ++i)
        {
            REQUIRE(qb.create("E" + std::to_string(i), spec(BetName::Field, 0, 5)).empty());
        }
        CHECK_FALSE(qb.create("One too many", spec(BetName::Field, 0, 5)).empty());
        CHECK(qb.plans().size() == QuickBets::MaxEntries);
    }
}

//----------------------------------------------------------------

TEST_CASE("QuickBets:addBet")
{
    QuickBets qb;
    REQUIRE(qb.create("Hi", spec(BetName::Place, 8, 24)).empty());
    bool replaced = true;

    SUBCASE("a new spot is added")
    {
        CHECK(qb.addBet("Hi", spec(BetName::Place, 9, 20), replaced).empty());
        CHECK_FALSE(replaced);
        REQUIRE(qb.plans()[0].specs.size() == 2);
        CHECK(qb.plans()[0].specs[1].pivot == 9u);
    }

    SUBCASE("the same spot changes the amount")
    {
        CHECK(qb.addBet("Hi", spec(BetName::Place, 8, 30), replaced).empty());
        CHECK(replaced);
        REQUIRE(qb.plans()[0].specs.size() == 1);
        CHECK(qb.plans()[0].specs[0].amount == 30u);
    }

    SUBCASE("odds are kept after the contract bets")
    {
        CHECK(qb.addBet("Hi", spec(BetName::PassLine, 0, 30, true), replaced).empty());
        CHECK(qb.addBet("Hi", spec(BetName::PassLine, 0, 10), replaced).empty());
        REQUIRE(qb.plans()[0].specs.size() == 3);
        CHECK_FALSE(qb.plans()[0].specs[1].isOdds);
        CHECK(qb.plans()[0].specs[2].isOdds);
    }

    SUBCASE("no such entry, or an unusable bet, is refused")
    {
        CHECK_FALSE(qb.addBet("Nope", spec(BetName::Field, 0, 5), replaced).empty());
        CHECK_FALSE(qb.addBet("Hi", spec(BetName::Field, 0, 0), replaced).empty());
        CHECK(qb.plans()[0].specs.size() == 1);
    }

    SUBCASE("a full entry refuses a new spot but still changes an old one")
    {
        for (unsigned i = 1; i < QuickBets::MaxSpecsPerEntry; ++i)
        {
            REQUIRE(qb.addBet("Hi", spec(BetName::Hardway, 4 + i, 5), replaced).empty());
        }
        REQUIRE(qb.plans()[0].specs.size() == QuickBets::MaxSpecsPerEntry);
        CHECK_FALSE(qb.addBet("Hi", spec(BetName::Field, 0, 5), replaced).empty());
        CHECK(qb.addBet("Hi", spec(BetName::Place, 8, 36), replaced).empty());
        CHECK(replaced);
    }
}

//----------------------------------------------------------------
