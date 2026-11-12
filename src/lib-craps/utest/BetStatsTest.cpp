//----------------------------------------------------------------
//
// File: BetStatsTest.cpp
//
// Tests for BetStats aggregation (tally) and roll dependent entries.
//
//----------------------------------------------------------------

#include <craps/BetStats.h>
#include <craps/CrapsBet.h>
#include <craps/DecisionRecord.h>
#include <craps/Dice.h>
#include <craps/Player.h>
#include <craps/RollStats.h>
#include <doctest/doctest.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>
#include <memory>

using namespace Craps;

//----------------------------------------------------------------

struct BetStatsFixture
{
    std::unique_ptr<Player> p1{Player::createPlayer("Player1")};
    Gen::ErrorPass ep;

    // Evaluate and record the outcome the same way CrapsTable does.
    void evalAndRecord(BetStats& bs, CrapsBet& bet, unsigned point,
                       unsigned d1, unsigned d2)
    {
        Dice dice;
        dice.set(d1, d2);
        DecisionRecord dr;
        REQUIRE(bet.testEvaluate(point, dice, dr, ep) ==
                Gen::ReturnCode::Success);
        CHECK(dr.roll == d1 + d2);
        if (dr.win > 0)
            bs.recordWin(bet, dr.win, dr.roll);
        if (dr.lose > 0)
            bs.recordLose(bet, dr.lose, dr.roll);
    }
};

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:tally come out and point scopes")
{
    BetStats bs;

    CrapsBet comeOutWin(p1.get(), BetName::PassLine, 10);
    evalAndRecord(bs, comeOutWin, 0, 3, 4);              // 7 wins

    CrapsBet pointWin(p1.get(), BetName::PassLine, 10);
    Dice dice;
    dice.set(3, 3);
    DecisionRecord dr;
    REQUIRE(pointWin.testEvaluate(0, dice, dr, ep) ==
            Gen::ReturnCode::Success);                   // Point is 6
    evalAndRecord(bs, pointWin, 6, 3, 3);                // 6 wins

    CrapsBet pointLose(p1.get(), BetName::PassLine, 10);
    dice.set(4, 4);
    REQUIRE(pointLose.testEvaluate(0, dice, dr, ep) ==
            Gen::ReturnCode::Success);                   // Point is 8
    evalAndRecord(bs, pointLose, 8, 3, 4);               // Seven out

    CHECK(bs.tally(BetName::PassLine, BetScope::ComeOut).wins.count   == 1);
    CHECK(bs.tally(BetName::PassLine, BetScope::ComeOut).losses.count == 0);
    CHECK(bs.tally(BetName::PassLine, BetScope::Points).wins.count    == 1);
    CHECK(bs.tally(BetName::PassLine, BetScope::Points).losses.count  == 1);
    CHECK(bs.tally(BetName::PassLine, BetScope::All).wins.count       == 2);
    CHECK(bs.tally(BetName::PassLine, BetScope::All).losses.count     == 1);
    CHECK(bs.tally(BetName::PassLine).wins.count                      == 2);
    CHECK(bs.tallyPoint(BetName::PassLine, 6).wins.count              == 1);
    CHECK(bs.tallyPoint(BetName::PassLine, 8).losses.count            == 1);
    CHECK(bs.tallyPoint(BetName::PassLine, 4).wins.count              == 0);
    CHECK(bs.tally(BetName::Come).wins.count                          == 0);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:tally do side and dont side")
{
    BetStats bs;

    CrapsBet pass(p1.get(), BetName::PassLine, 10);
    evalAndRecord(bs, pass, 0, 3, 4);                    // Pass wins on 7

    CrapsBet dont(p1.get(), BetName::DontPass, 10);
    evalAndRecord(bs, dont, 0, 3, 4);                    // Dont loses on 7

    CHECK(bs.tallyDoSide(BetScope::ComeOut).wins.count     == 1);
    CHECK(bs.tallyDoSide(BetScope::ComeOut).losses.count   == 0);
    CHECK(bs.tallyDontSide(BetScope::ComeOut).wins.count   == 0);
    CHECK(bs.tallyDontSide(BetScope::ComeOut).losses.count == 1);
    CHECK(bs.tallyDoSide(BetScope::Points).wins.count      == 0);
    CHECK(bs.tallyDontSide().losses.count                  == 1);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:field recorded overall and by number")
{
    BetStats bs;

    CrapsBet f1(p1.get(), BetName::Field, 10);
    evalAndRecord(bs, f1, 0, 1, 1);                      // 2 wins
    CrapsBet f2(p1.get(), BetName::Field, 10);
    evalAndRecord(bs, f2, 6, 4, 5);                      // 9 wins
    CrapsBet f3(p1.get(), BetName::Field, 10);
    evalAndRecord(bs, f3, 6, 3, 4);                      // 7 loses
    CrapsBet f4(p1.get(), BetName::Field, 10);
    evalAndRecord(bs, f4, 6, 3, 4);                      // 7 loses
    CrapsBet f5(p1.get(), BetName::Field, 10);
    evalAndRecord(bs, f5, 0, 2, 3);                      // 5 loses

    CHECK(bs.tally(BetName::Field).wins.count            == 2);
    CHECK(bs.tally(BetName::Field).losses.count          == 3);
    CHECK(bs.tallyRoll(BetName::Field, 2).wins.count     == 1);
    CHECK(bs.tallyRoll(BetName::Field, 9).wins.count     == 1);
    CHECK(bs.tallyRoll(BetName::Field, 7).losses.count   == 2);
    CHECK(bs.tallyRoll(BetName::Field, 5).losses.count   == 1);
    CHECK(bs.tallyRoll(BetName::Field, 7).wins.count     == 0);
    CHECK(bs.tallyRoll(BetName::PassLine, 7).wins.count  == 0);

    // Field is recorded once, under the number rolled.
    CHECK(bs.byBet["Field2"].wins.count  == 1);
    CHECK(bs.byBet["Field7"].losses.count == 2);
    CHECK(bs.totNumWinsAllBets           == 2);
    CHECK(bs.totNumLoseAllBets           == 3);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:any seven recorded overall and by number")
{
    BetStats bs;
    CrapsBet s1(p1.get(), BetName::AnySeven, 5);
    evalAndRecord(bs, s1, 0, 3, 4);                      // 7 wins
    CrapsBet s2(p1.get(), BetName::AnySeven, 5);
    evalAndRecord(bs, s2, 0, 3, 3);                      // 6 loses

    CHECK(bs.tally(BetName::AnySeven).wins.count           == 1);
    CHECK(bs.tally(BetName::AnySeven).losses.count         == 1);
    CHECK(bs.tallyRoll(BetName::AnySeven, 7).wins.count    == 1);
    CHECK(bs.tallyRoll(BetName::AnySeven, 6).losses.count  == 1);
    CHECK(bs.tallyRoll(BetName::AnySeven, 6).wins.count    == 0);
    CHECK(bs.totNumWinsAllBets                             == 1);
}

//----------------------------------------------------------------

TEST_CASE("BetStats:roll dependent entries survive merge")
{
    BetStats a, b;
    a.byBet["Field2"].wins.count = 1;
    b.byBet["Field2"].wins.count = 2;
    b.byBet["Horn@11"].wins.count = 1;
    a.merge(b);
    CHECK(a.tallyRoll(BetName::Field, 2).wins.count == 3);
    CHECK(a.tallyRoll(BetName::Horn, 11).wins.count == 1);
}

//----------------------------------------------------------------

TEST_CASE("RollStats:simulatedCounts")
{
    RollStats rs;
    rs.numberCounts[7].bump();
    rs.numberCounts[7].bump();
    rs.numberCounts[11].bump();
    rs.numberCounts[2].bump();

    CHECK(rs.simulatedCounts(BetName::AnySeven).wins   == 2);
    CHECK(rs.simulatedCounts(BetName::AnySeven).losses == 2);
    CHECK(rs.simulatedCounts(BetName::AnyCraps).wins   == 1);
    CHECK(rs.simulatedCounts(BetName::AnyCraps).losses == 3);
    CHECK(rs.simulatedCounts(BetName::Place, 6).wins   == 0);
    CHECK(rs.simulatedCounts(BetName::Place, 6).losses == 2);
}
