//----------------------------------------------------------------
//
// File: CrapsBetFlagsTest.cpp
//
// Bet flags: standing state, one roll override, when a bet is live,
// what a bet that is not live does, and CrapsTable::setBetFlags().
//
//----------------------------------------------------------------

#include <doctest/doctest.h>
#include <memory>
#include <craps/CrapsBet.h>
#include <craps/CrapsTable.h>
#include <craps/Dice.h>
#include <craps/DecisionRecord.h>
#include <craps/Player.h>
#include <gen/ErrorPass.h>

using namespace Craps;

//----------------------------------------------------------------

struct BetFlagsFixture
{
    Craps::TableConfig config;
    Player*            p1 = nullptr;

    BetFlagsFixture()
    {
        p1 = Player::createPlayer("Player1");
        config.maxSessions    = 50;
        config.maxRecentRolls = 25;
    }

    ~BetFlagsFixture()
    {
        delete p1;
    }

    // Evaluate a bet for a roll and return the decision record
    DecisionRecord eval(CrapsBet& bet, unsigned point, unsigned d1, unsigned d2)
    {
        DecisionRecord dr;
        Gen::ErrorPass ep;
        Dice dice;
        dice.set(d1, d2);
        const bool ok = bet.testEvaluate(point, dice, dr, ep) ==
                        Gen::ReturnCode::Success;
        CHECK(ok);
        return dr;
    }
};

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: a new bet has default flags")
{
    CrapsBet place(p1, BetName::Place, 12, 6);
    CHECK(place.flagStanding() == FlagStanding::Default);
    CHECK(place.flagOneRoll()  == FlagOneRoll::None);
    CHECK(place.flagsAreDefault());
    CHECK(place.offComeOutRoll());

    CrapsBet hardway(p1, BetName::Hardway, 5, 4);
    CHECK(hardway.flagsAreDefault());
    CHECK_FALSE(hardway.offComeOutRoll());
    CHECK(hardway.hardwayWorking());
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: isLive truth table")
{
    CrapsBet place(p1, BetName::Place, 12, 6);
    CrapsBet hardway(p1, BetName::Hardway, 5, 4);

    // Default: Place is live except on come out; Hardway is always live
    CHECK_FALSE(place.isLive(0));
    CHECK(place.isLive(4));
    CHECK(hardway.isLive(0));
    CHECK(hardway.isLive(4));

    // Standing Working: live on every roll
    place.setFlagStanding(FlagStanding::Working);
    CHECK(place.isLive(0));
    CHECK(place.isLive(4));
    CHECK_FALSE(place.offComeOutRoll());

    // Standing Off: never live
    place.setFlagStanding(FlagStanding::Off);
    hardway.setFlagStanding(FlagStanding::Off);
    CHECK_FALSE(place.isLive(0));
    CHECK_FALSE(place.isLive(4));
    CHECK_FALSE(hardway.isLive(0));
    CHECK_FALSE(hardway.isLive(4));
    CHECK_FALSE(hardway.hardwayWorking());

    // The override beats the standing state, then goes away
    place.setFlagOneRoll(FlagOneRoll::Working);
    CHECK(place.isLive(0));
    CHECK(place.isLive(4));
    CHECK(place.flagStanding() == FlagStanding::Off);   // not touched
    place.clearFlagOneRoll();
    CHECK_FALSE(place.isLive(4));

    place.setFlagStanding(FlagStanding::Working);
    place.setFlagOneRoll(FlagOneRoll::Off);
    CHECK_FALSE(place.isLive(0));
    CHECK_FALSE(place.isLive(4));
    place.clearFlagOneRoll();
    CHECK(place.isLive(4));
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: the older setters set the standing state")
{
    CrapsBet place(p1, BetName::Place, 12, 6);
    place.setOnComeOutRoll();
    CHECK(place.flagStanding() == FlagStanding::Working);
    place.setOffComeOutRoll();
    CHECK(place.flagStanding() == FlagStanding::Default);

    CrapsBet hardway(p1, BetName::Hardway, 5, 4);
    hardway.setHardwayOff();
    CHECK(hardway.flagStanding() == FlagStanding::Off);
    hardway.setHardwayOn();
    CHECK(hardway.flagStanding() == FlagStanding::Default);

    // Not a Hardway: silently nothing
    place.setHardwayOff();
    CHECK(place.flagStanding() == FlagStanding::Default);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: canTakeFlag follows casino practice")
{
    CHECK(CrapsBet(p1, BetName::Place,   12, 6).canTakeFlag());
    CHECK(CrapsBet(p1, BetName::Buy,     20, 4).canTakeFlag());
    CHECK(CrapsBet(p1, BetName::Lay,     20, 4).canTakeFlag());
    CHECK(CrapsBet(p1, BetName::Hardway,  5, 8).canTakeFlag());

    CHECK_FALSE(CrapsBet(p1, BetName::Field,    10).canTakeFlag());
    CHECK_FALSE(CrapsBet(p1, BetName::AnyCraps,  1).canTakeFlag());
    CHECK_FALSE(CrapsBet(p1, BetName::Horn,      4).canTakeFlag());

    // Line and Come bets: only with odds
    CrapsBet pass(p1, BetName::PassLine, 10, 6);
    CHECK_FALSE(pass.canTakeFlag());
    pass.testSetOddsAmount(30);
    CHECK(pass.canTakeFlag());

    CrapsBet comeBet(p1, BetName::Come, 10, 4);
    CHECK_FALSE(comeBet.canTakeFlag());
    comeBet.testSetOddsAmount(30);
    CHECK(comeBet.canTakeFlag());
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: Place bet that is not live is kept")
{
    // Place 6 wins on a 6, loses on a 7
    CrapsBet b(p1, BetName::Place, 12, 6);

    // Default, point 4: live, wins on 6
    CHECK(eval(b, 4, 4, 2).decision);

    // Off: kept on a 6 and kept on a 7
    b.setFlagStanding(FlagStanding::Off);
    CHECK_FALSE(eval(b, 4, 4, 2).decision);
    CHECK_FALSE(eval(b, 4, 4, 3).decision);

    // Working next roll beats Off
    b.setFlagOneRoll(FlagOneRoll::Working);
    CHECK(eval(b, 4, 4, 2).decision);

    // Default on a come out: not live; Working: live
    CrapsBet c(p1, BetName::Place, 12, 6);
    CHECK_FALSE(eval(c, 0, 4, 2).decision);
    c.setFlagStanding(FlagStanding::Working);
    CHECK(eval(c, 0, 4, 2).decision);

    // Off next roll beats Working
    c.setFlagOneRoll(FlagOneRoll::Off);
    CHECK_FALSE(eval(c, 4, 4, 2).decision);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: Buy and Lay that are not live are kept")
{
    CrapsBet buy(p1, BetName::Buy, 20, 4);
    CHECK(eval(buy, 6, 2, 2).decision);
    buy.setFlagStanding(FlagStanding::Off);
    CHECK_FALSE(eval(buy, 6, 2, 2).decision);

    CrapsBet lay(p1, BetName::Lay, 20, 4);
    CHECK(eval(lay, 6, 4, 3).decision);         // 7 wins a Lay
    lay.setFlagStanding(FlagStanding::Off);
    CHECK_FALSE(eval(lay, 6, 4, 3).decision);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: Hardway off is kept, back on decides")
{
    CrapsBet b(p1, BetName::Hardway, 5, 4);
    CHECK(eval(b, 0, 2, 2).decision);           // live on come out by default

    b.setFlagStanding(FlagStanding::Off);
    CHECK_FALSE(eval(b, 6, 2, 2).decision);
    CHECK_FALSE(eval(b, 6, 4, 3).decision);     // not even a 7

    b.setFlagStanding(FlagStanding::Default);
    b.setFlagOneRoll(FlagOneRoll::Off);
    CHECK_FALSE(eval(b, 6, 2, 2).decision);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: odds that are not live are returned")
{
    // Pass Line on the 6 with $100 odds. Live: odds pay 6:5.
    CrapsBet live(p1, BetName::PassLine, 100, 6);
    live.testSetOddsAmount(100);
    auto dr = eval(live, 6, 3, 3);
    CHECK(dr.win == 220);
    CHECK(dr.returnToPlayer == 0);

    // Odds Off: the contract bet wins and the odds come back
    CrapsBet off(p1, BetName::PassLine, 100, 6);
    off.testSetOddsAmount(100);
    off.setFlagStanding(FlagStanding::Off);
    dr = eval(off, 6, 3, 3);
    CHECK(dr.win == 100);
    CHECK(dr.returnToPlayer == 100);

    // Odds Off and a seven out: the contract bet loses, the odds come back
    CrapsBet seven(p1, BetName::PassLine, 100, 6);
    seven.testSetOddsAmount(100);
    seven.setFlagOneRoll(FlagOneRoll::Off);
    dr = eval(seven, 6, 4, 3);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 100);

    // Live on a seven out: odds lose too
    CrapsBet liveSeven(p1, BetName::PassLine, 100, 6);
    liveSeven.testSetOddsAmount(100);
    dr = eval(liveSeven, 6, 4, 3);
    CHECK(dr.lose == 200);
    CHECK(dr.returnToPlayer == 0);

    // Don't Pass odds too
    CrapsBet dont(p1, BetName::DontPass, 100, 6);
    dont.testSetOddsAmount(120);
    dont.setFlagStanding(FlagStanding::Off);
    dr = eval(dont, 6, 4, 3);       // seven wins for Don't Pass
    CHECK(dr.win == 100);
    CHECK(dr.returnToPlayer == 120);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: Come odds still follow the come out rule")
{
    // Odds off on the come out by default and returned: existing behavior
    CrapsBet b(p1, BetName::Come, 100, 4);
    b.testSetOddsAmount(200);
    auto dr = eval(b, 0, 6, 1);
    CHECK(dr.lose == 100);
    CHECK(dr.returnToPlayer == 200);

    // Working on the come out: odds lose too
    CrapsBet w(p1, BetName::Come, 100, 4);
    w.testSetOddsAmount(200);
    w.setFlagOneRoll(FlagOneRoll::Working);
    dr = eval(w, 0, 6, 1);
    CHECK(dr.lose == 300);
    CHECK(dr.returnToPlayer == 0);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: flags take part in equality")
{
    CrapsBet a(p1, BetName::Place, 12, 6);
    CrapsBet b = a;
    CHECK(a == b);
    b.setFlagStanding(FlagStanding::Off);
    CHECK_FALSE(a == b);
    b.setFlagStanding(FlagStanding::Default);
    b.setFlagOneRoll(FlagOneRoll::Off);
    CHECK_FALSE(a == b);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: CrapsTable::setBetFlags")
{
    CrapsTable t("Table-1", config);
    REQUIRE(t.addPlayer(*p1));
    Gen::ErrorPass ep;

    auto place = std::make_shared<CrapsBet>(p1, BetName::Place, 12, 6);
    REQUIRE(t.addBet(place, ep) == Gen::ReturnCode::Success);

    // A Place bet takes flags
    CHECK(t.setBetFlags(place, FlagStanding::Off, FlagOneRoll::None, ep) ==
          Gen::ReturnCode::Success);
    CHECK(place->flagStanding() == FlagStanding::Off);

    // Both at once
    CHECK(t.setBetFlags(place, FlagStanding::Working, FlagOneRoll::Off, ep) ==
          Gen::ReturnCode::Success);
    CHECK(place->flagStanding() == FlagStanding::Working);
    CHECK(place->flagOneRoll()  == FlagOneRoll::Off);

    // A Field bet does not
    auto field = std::make_shared<CrapsBet>(p1, BetName::Field, 10, 0);
    REQUIRE(t.addBet(field, ep) == Gen::ReturnCode::Success);
    Gen::ErrorPass epField;
    CHECK(t.setBetFlags(field, FlagStanding::Off, FlagOneRoll::None, epField) ==
          Gen::ReturnCode::Fail);
    CHECK_FALSE(epField.description().empty());
    CHECK(field->flagsAreDefault());

    // A Pass Line bet without odds does not
    auto pass = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
    REQUIRE(t.addBet(pass, ep) == Gen::ReturnCode::Success);
    Gen::ErrorPass epPass;
    CHECK(t.setBetFlags(pass, FlagStanding::Off, FlagOneRoll::None, epPass) ==
          Gen::ReturnCode::Fail);

    // A bet that is not on the table does not
    auto stray = std::make_shared<CrapsBet>(p1, BetName::Place, 12, 8);
    Gen::ErrorPass epStray;
    CHECK(t.setBetFlags(stray, FlagStanding::Off, FlagOneRoll::None, epStray) ==
          Gen::ReturnCode::Fail);

    // A null bet does not
    BetPtr none;
    Gen::ErrorPass epNone;
    CHECK(t.setBetFlags(none, FlagStanding::Off, FlagOneRoll::None, epNone) ==
          Gen::ReturnCode::Fail);

    // Not when betting is closed
    t.testSetBettingOpen(false);
    Gen::ErrorPass epClosed;
    CHECK(t.setBetFlags(place, FlagStanding::Off, FlagOneRoll::None, epClosed) ==
          Gen::ReturnCode::Fail);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: odds flags need odds and go when the odds go")
{
    CrapsTable t("Table-1", config);
    REQUIRE(t.addPlayer(*p1));
    Gen::ErrorPass ep;

    auto pass = std::make_shared<CrapsBet>(p1, BetName::PassLine, 10, 0);
    REQUIRE(t.addBet(pass, ep) == Gen::ReturnCode::Success);
    t.testSetState(4, 3, 1);
    pass->testSetPivot(4);
    REQUIRE(t.setOddsAmount(pass, 30, ep) == Gen::ReturnCode::Success);

    CHECK(t.setBetFlags(pass, FlagStanding::Off, FlagOneRoll::Working, ep) ==
          Gen::ReturnCode::Success);

    // Taking the odds down takes the flags with them
    CHECK(t.setOddsAmount(pass, 0, ep) == Gen::ReturnCode::Success);
    CHECK(pass->flagsAreDefault());
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetFlagsFixture, "BetFlags: the one roll override is cleared by a roll")
{
    CrapsTable t("Table-1", config);
    REQUIRE(p1->joinTable(t));      // The player must own the bet for a roll
    Gen::ErrorPass ep;

    t.testSetState(4, 2, 2);
    auto made = p1->testMakeBet(BetName::Place, 12, 6);
    REQUIRE(made);
    BetPtr place = made.value();

    REQUIRE(t.setBetFlags(place, FlagStanding::Working, FlagOneRoll::Off, ep) ==
            Gen::ReturnCode::Success);

    t.testRollDice(2, 3);       // a 5: nothing decided for Place 6

    CHECK(place->flagStanding() == FlagStanding::Working);   // stays
    CHECK(place->flagOneRoll()  == FlagOneRoll::None);        // gone
}

//----------------------------------------------------------------
