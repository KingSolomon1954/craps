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
#include <yaml-cpp/yaml.h>

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
    rs.pointNumberCounts[7].bump();   // Place/Buy/Lay only see point rolls
    rs.pointNumberCounts[7].bump();

    CHECK(rs.simulatedCounts(BetName::AnySeven).wins   == 2);
    CHECK(rs.simulatedCounts(BetName::AnySeven).losses == 2);
    CHECK(rs.simulatedCounts(BetName::AnyCraps).wins   == 1);
    CHECK(rs.simulatedCounts(BetName::AnyCraps).losses == 3);
    CHECK(rs.simulatedCounts(BetName::Place, 6).wins   == 0);
    CHECK(rs.simulatedCounts(BetName::Place, 6).losses == 2);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:odds recorded separately from contract")
{
    BetStats bs;

    // PassLine 10 with 20 odds on the 6 (6:5 pays 24)
    CrapsBet b(p1.get(), BetName::PassLine, 10);
    Dice dice;
    dice.set(3, 3);
    DecisionRecord dr;
    REQUIRE(b.testEvaluate(0, dice, dr, ep) == Gen::ReturnCode::Success);
    b.testSetOddsAmount(20);
    REQUIRE(b.testEvaluate(6, dice, dr, ep) == Gen::ReturnCode::Success);
    REQUIRE(dr.decision);
    CHECK(dr.oddsWin > 0);
    CHECK(dr.win == 10 + dr.oddsWin);
    bs.recordWin(dr);

    const auto w = bs.tallyPoint(BetName::PassLine, 6).wins;
    CHECK(w.count      == 1);
    CHECK(w.amountBet  == 30);
    CHECK(w.oddsBet    == 20);
    CHECK(w.oddsAmount == dr.oddsWin);
    CHECK(w.amount     == dr.win);

    // Odds that lose
    CrapsBet l(p1.get(), BetName::PassLine, 10);
    dice.set(4, 4);
    REQUIRE(l.testEvaluate(0, dice, dr, ep) == Gen::ReturnCode::Success);
    l.testSetOddsAmount(20);
    dice.set(3, 4);
    REQUIRE(l.testEvaluate(8, dice, dr, ep) == Gen::ReturnCode::Success);
    CHECK(dr.lose     == 30);
    CHECK(dr.oddsLose == 20);
    bs.recordLose(dr);
    const auto lo = bs.tallyPoint(BetName::PassLine, 8).losses;
    CHECK(lo.amountBet  == 30);
    CHECK(lo.oddsBet    == 20);
    CHECK(lo.oddsAmount == 20);
    CHECK(bs.tally(BetName::PassLine).wins.oddsBet   == 20);
    CHECK(bs.tally(BetName::PassLine).losses.oddsBet == 20);
}

//----------------------------------------------------------------

TEST_CASE("BetStats:BetStat odds fields yaml")
{
    BetStat s;
    s.count = 2; s.amountBet = 50; s.oddsBet = 20; s.oddsAmount = 24;
    BetStat t;
    t.fromYAML(s.toYAML());
    CHECK(t == s);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:hardway losses by cause")
{
    BetStats bs;

    CrapsBet w(p1.get(), BetName::Hardway, 10, 6);
    evalAndRecord(bs, w, 6, 3, 3);                       // Hard 6 wins
    CrapsBet e1(p1.get(), BetName::Hardway, 10, 6);
    evalAndRecord(bs, e1, 6, 2, 4);                      // Easy 6 loses
    CrapsBet e2(p1.get(), BetName::Hardway, 10, 6);
    evalAndRecord(bs, e2, 6, 1, 5);                      // Easy 6 loses
    CrapsBet s1(p1.get(), BetName::Hardway, 10, 6);
    evalAndRecord(bs, s1, 6, 3, 4);                      // Seven loses
    CrapsBet o(p1.get(), BetName::Hardway, 10, 8);
    evalAndRecord(bs, o, 6, 3, 4);                       // Seven loses the 8

    CHECK(bs.tallyPoint(BetName::Hardway, 6).wins.count   == 1);
    CHECK(bs.tallyPoint(BetName::Hardway, 6).losses.count == 3);
    CHECK(bs.tallyHardwayLosses(6).easy.count  == 2);
    CHECK(bs.tallyHardwayLosses(6).seven.count == 1);
    CHECK(bs.tallyHardwayLosses(8).easy.count  == 0);
    CHECK(bs.tallyHardwayLosses(8).seven.count == 1);
    CHECK(bs.tallyHardwayLosses(6).easy.amount == 20);

    // The cause entries are extra, totals are not double counted
    CHECK(bs.totNumLoseAllBets == 4);
    CHECK(bs.totNumWinsAllBets == 1);
}

//----------------------------------------------------------------

TEST_CASE("BetStats:classifyRoll")
{
    using R = RollOutcome;
    CHECK(BetStats::classifyRoll(BetName::World, 7)    == R::BreakEven);
    CHECK(BetStats::classifyRoll(BetName::World, 2)    == R::PartialWin);
    CHECK(BetStats::classifyRoll(BetName::World, 6)    == R::EntireLoss);
    CHECK(BetStats::classifyRoll(BetName::Horn, 11)    == R::PartialWin);
    CHECK(BetStats::classifyRoll(BetName::Horn, 7)     == R::EntireLoss);
    CHECK(BetStats::classifyRoll(BetName::CandE, 3)    == R::PartialWin);
    CHECK(BetStats::classifyRoll(BetName::CandE, 9)    == R::EntireLoss);
    CHECK(BetStats::classifyRoll(BetName::AnySeven, 7) == R::CleanWin);
    CHECK(BetStats::classifyRoll(BetName::AnyCraps, 11)== R::EntireLoss);
    CHECK(BetStats::classifyRoll(BetName::Field, 9)    == R::CleanWin);
    CHECK(BetStats::classifyRoll(BetName::Field, 6)    == R::EntireLoss);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:tallyRollOutcome")
{
    BetStats bs;
    CrapsBet h1(p1.get(), BetName::Horn, 20);
    evalAndRecord(bs, h1, 0, 1, 1);                      // 2 partial win
    CrapsBet h2(p1.get(), BetName::Horn, 20);
    evalAndRecord(bs, h2, 0, 5, 6);                      // 11 partial win
    CrapsBet h3(p1.get(), BetName::Horn, 20);
    evalAndRecord(bs, h3, 0, 3, 3);                      // 6 entire loss
    CrapsBet h4(p1.get(), BetName::Horn, 20);
    evalAndRecord(bs, h4, 0, 3, 4);                      // 7 entire loss

    CHECK(bs.tallyRollOutcome(BetName::Horn, RollOutcome::PartialWin).wins.count   == 2);
    CHECK(bs.tallyRollOutcome(BetName::Horn, RollOutcome::PartialWin).losses.count == 0);
    CHECK(bs.tallyRollOutcome(BetName::Horn, RollOutcome::EntireLoss).losses.count == 2);
    CHECK(bs.tallyRollOutcome(BetName::Horn, RollOutcome::EntireLoss).wins.count   == 0);
    CHECK(bs.tallyRollOutcome(BetName::PassLine, RollOutcome::CleanWin).wins.count == 0);
}

//----------------------------------------------------------------

TEST_CASE_FIXTURE(BetStatsFixture, "BetStats:per roll counters")
{
    BetStats bs;

    // Roll 1: two bets kept
    CrapsBet k1(p1.get(), BetName::PassLine, 10);
    CrapsBet k2(p1.get(), BetName::Come, 20);
    bs.recordKeep(k1);
    bs.recordKeep(k2);
    CHECK(bs.numBetsOneRoll.total     == 2);
    CHECK(bs.numBetsOneRoll.max       == 2);
    CHECK(bs.amtBetsOneRoll.total     == 30);
    CHECK(bs.amtBetsOneRoll.max       == 30);
    CHECK(bs.numBetsKeepOneRoll.total == 2);
    CHECK(bs.amtBetsKeepOneRoll.total == 30);

    // Roll 2: one bet decided
    bs.resetRollCounts();
    CrapsBet w(p1.get(), BetName::AnySeven, 5);
    bs.recordWin(w, 20, 7);
    CHECK(bs.numBetsOneRoll.total     == 3);    // Bets evaluated, all rolls
    CHECK(bs.numBetsOneRoll.max       == 2);    // Most in a single roll
    CHECK(bs.amtBetsOneRoll.total     == 35);
    CHECK(bs.amtBetsOneRoll.max       == 30);
    CHECK(bs.numBetsWinOneRoll.total  == 1);
    CHECK(bs.numBetsWinOneRoll.max    == 1);
    CHECK(bs.amtBetsWinOneRoll.total  == 20);
}

//----------------------------------------------------------------

TEST_CASE("BetStats:rollActivity yaml names and round trip")
{
    BetStats a;
    a.numBetsOneRoll.total      = 52;
    a.numBetsOneRoll.max        = 5;
    a.numBetsWinOneRoll.total   = 9;
    a.amtBetsOneRoll.total      = 665;
    a.amtBetsOneRoll.max        = 90;
    a.amtBetsLoseOneRoll.total  = 225;

    const YAML::Node node = a.toYAML();
    const YAML::Node ra   = node["rollActivity"];
    CHECK(ra["betsEvaluated"]["allRolls"].as<unsigned>()    == 52);
    CHECK(ra["betsEvaluated"]["busiestRoll"].as<unsigned>() == 5);
    CHECK(ra["betsWon"]["allRolls"].as<unsigned>()          == 9);
    CHECK(ra["amounts"]["evaluated"]["allRolls"].as<unsigned>()    == 665);
    CHECK(ra["amounts"]["evaluated"]["busiestRoll"].as<unsigned>() == 90);
    CHECK(ra["amounts"]["lost"]["allRolls"].as<unsigned>()  == 225);
    CHECK_FALSE(node["perRoll"].IsDefined());

    BetStats b;
    b.fromYAML(node);
    CHECK(b.numBetsOneRoll.total  == 52);
    CHECK(b.numBetsOneRoll.max    == 5);
    CHECK(b.amtBetsOneRoll.max    == 90);
    CHECK(b.amtBetsLoseOneRoll.total == 225);
}
