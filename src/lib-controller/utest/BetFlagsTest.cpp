//----------------------------------------------------------------
//
// File: BetFlagsTest.cpp
//
//----------------------------------------------------------------

#include <controller/BetFlags.h>
#include <controller/UndoBet.h>
#include <controller/UndoManager.h>
#include <craps/CrapsTable.h>
#include <craps/Player.h>
#include <craps/TableConfig.h>
#include <doctest/doctest.h>
#include <memory>

using namespace Ctrl;
using namespace Ctrl::BetFlags;
using Craps::CrapsBet;
using Craps::FlagOneRoll;
using Craps::FlagStanding;

namespace {

struct FlagsFixture
{
    FlagsFixture()
    {
        Craps::TableConfig tableConfig;
        tableConfig.maxSessions    = 50;
        tableConfig.maxRecentRolls = 25;
        table  = std::make_unique<Craps::CrapsTable>("Table-1", tableConfig);
        player.reset(Craps::Player::createPlayer("flags-player"));
        REQUIRE(player->joinTable(*table));
    }

    Craps::BetPtr place(BetName name, Gen::Money amount, unsigned pivot)
    {
        auto r = player->makeOrChangeBet(name, amount, pivot);
        REQUIRE(r);
        return r.value().bet;
    }

    std::unique_ptr<Craps::CrapsTable> table;
    std::unique_ptr<Craps::Player>     player;
};

} // namespace

//----------------------------------------------------------------

TEST_CASE("BetFlags: categories")
{
    CHECK(categoryOf(BetName::PassLine) == Category::Odds);
    CHECK(categoryOf(BetName::DontPass) == Category::Odds);
    CHECK(categoryOf(BetName::Come)     == Category::Odds);
    CHECK(categoryOf(BetName::DontCome) == Category::Odds);
    CHECK(categoryOf(BetName::Place)    == Category::Place);
    CHECK(categoryOf(BetName::Buy)      == Category::Buy);
    CHECK(categoryOf(BetName::Lay)      == Category::Lay);
    CHECK(categoryOf(BetName::Hardway)  == Category::Hardway);
    CHECK(categoryOf(BetName::Field)    == Category::None);
    CHECK(categoryOf(BetName::Horn)     == Category::None);
}

//----------------------------------------------------------------

TEST_CASE("BetFlags: scope and category")
{
    CHECK(inScope(Scope::All,     Category::Odds));
    CHECK(inScope(Scope::All,     Category::Hardway));
    CHECK_FALSE(inScope(Scope::All, Category::None));
    CHECK(inScope(Scope::Odds,    Category::Odds));
    CHECK_FALSE(inScope(Scope::Odds, Category::Place));
    CHECK(inScope(Scope::Place,   Category::Place));
    CHECK_FALSE(inScope(Scope::Place, Category::Buy));
    CHECK(inScope(Scope::Buy,     Category::Buy));
    CHECK(inScope(Scope::Lay,     Category::Lay));
    CHECK(inScope(Scope::Hardway, Category::Hardway));
    CHECK_FALSE(inScope(Scope::Bet, Category::Place));  // caller has the id
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(FlagsFixture, "BetFlags: scope for a bet needs a flaggable bet")
{
    auto pass  = place(BetName::PassLine, 10, 0);
    auto field = place(BetName::Field, 10, 0);
    auto p6    = place(BetName::Place, 12, 6);

    CHECK_FALSE(inScope(Scope::All,  *pass));     // no odds yet
    CHECK_FALSE(inScope(Scope::All,  *field));
    CHECK(inScope(Scope::All,   *p6));
    CHECK(inScope(Scope::Place, *p6));
    CHECK_FALSE(inScope(Scope::Odds, *p6));

    // Odds need a point, which these tests cannot set up; the model
    // tests cover pass line bets with odds. A pass line bet with no odds
    // has nothing to flag.
    CHECK_FALSE(inScope(Scope::Odds, *pass));
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(FlagsFixture, "BetFlags: what each change does")
{
    auto p6 = place(BetName::Place, 12, 6);

    auto s = newState(Change::NotWorking, *p6);
    CHECK(s.first == FlagStanding::Off);
    CHECK(s.second == FlagOneRoll::None);

    s = newState(Change::Working, *p6);
    CHECK(s.first == FlagStanding::Working);
    CHECK(s.second == FlagOneRoll::None);

    s = newState(Change::TableDefault, *p6);
    CHECK(s.first == FlagStanding::Default);
    CHECK(s.second == FlagOneRoll::None);

    // Next roll changes leave the standing state alone
    p6->setFlagStanding(FlagStanding::Off);
    s = newState(Change::WorkingNextRoll, *p6);
    CHECK(s.first == FlagStanding::Off);
    CHECK(s.second == FlagOneRoll::Working);

    p6->setFlagStanding(FlagStanding::Working);
    s = newState(Change::NotWorkingNextRoll, *p6);
    CHECK(s.first == FlagStanding::Working);
    CHECK(s.second == FlagOneRoll::Off);

    // A standing change drops the override
    p6->setFlagOneRoll(FlagOneRoll::Off);
    s = newState(Change::Working, *p6);
    CHECK(s.second == FlagOneRoll::None);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(FlagsFixture, "BetFlags: words")
{
    auto p9 = place(BetName::Place, 20, 9);
    CHECK(betLabel(*p9) == "Place 9 $20");
    CHECK(flagText(*p9) == "Table Default");

    p9->setFlagStanding(FlagStanding::Working);
    p9->setFlagOneRoll(FlagOneRoll::Off);
    CHECK(flagText(*p9) == "Working, Not-Working Next Roll");

    p9->setFlagStanding(FlagStanding::Off);
    p9->setFlagOneRoll(FlagOneRoll::Working);
    CHECK(flagText(*p9) == "Not-Working, Working Next Roll");

    auto hw = place(BetName::Hardway, 5, 4);
    CHECK(betLabel(*hw) == "Hardway 4 $5");

    CHECK(changeText(Change::NotWorking)         == "Not-Working");
    CHECK(changeText(Change::NotWorkingNextRoll) == "Not-Working Next Roll");
    CHECK(changeText(Change::Working)            == "Working");
    CHECK(changeText(Change::WorkingNextRoll)    == "Working Next Roll");
    CHECK(changeText(Change::TableDefault)       == "Table Default");
    CHECK(scopeText(Scope::Odds)  == "odds bets");
    CHECK(scopeText(Scope::Place) == "place bets");
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(FlagsFixture, "BetFlags: Player::setBetFlags and undo")
{
    auto p6 = place(BetName::Place, 12, 6);
    Gen::ErrorPass ep;

    UndoBetFlagsChanged undo(p6);
    REQUIRE(player->setBetFlags(p6, FlagStanding::Off, FlagOneRoll::Working, ep) ==
            Gen::ReturnCode::Success);
    CHECK(p6->flagStanding() == FlagStanding::Off);
    CHECK(p6->flagOneRoll()  == FlagOneRoll::Working);

    CHECK(undo.undo(ep) == Gen::ReturnCode::Success);
    CHECK(p6->flagsAreDefault());
    CHECK_FALSE(undo.describe().empty());
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(FlagsFixture, "BetFlags: Player::setBetFlags refuses what the table refuses")
{
    auto field = place(BetName::Field, 10, 0);
    Gen::ErrorPass ep;
    CHECK(player->setBetFlags(field, FlagStanding::Off, FlagOneRoll::None, ep) ==
          Gen::ReturnCode::Fail);
    CHECK(field->flagsAreDefault());

    Craps::BetPtr none;
    Gen::ErrorPass epNone;
    CHECK(player->setBetFlags(none, FlagStanding::Off, FlagOneRoll::None, epNone) ==
          Gen::ReturnCode::Fail);
}

//----------------------------------------------------------------
