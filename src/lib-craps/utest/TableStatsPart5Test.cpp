//----------------------------------------------------------------
//
// File: TableStatsPart5Test.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <craps/DecisionRecord.h>
#include <craps/CrapsBet.h>
#include <craps/Dice.h>
#include <doctest/doctest.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("TableStats:betstats:win1")
{
    Gen::ErrorPass ep;
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;
    DecisionRecord dr1, dr2;

    CrapsBet b1("Player1", BetName::PassLine, 10);
    CrapsBet b2("Player2", BetName::PassLine, 10);
    point = 0; dice.set(3,4);
    CHECK(b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordWin(b1, b1.contractAmount());
    ts.recordWin(b2, b2.contractAmount());

    CHECK(ts.betsWinLose.wins["PassLine"].count       == 2);
    CHECK(ts.betsWinLose.wins["PassLine"].amountBet   == 20);
    CHECK(ts.betsWinLose.wins["PassLine"].amount      == 20);
    CHECK(ts.betsWinLose.wins["PassLine"].totDistance == 2);

    CHECK(ts.betsWinLose.lose["PassLine"].count       == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amountBet   == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amount      == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].totDistance == 0);

    CHECK(ts.totNumBetsAllBets         == 2);
    CHECK(ts.totAmtAllBets             == 20);

    CHECK(ts.numBetsWinOneRoll.total   == 2);
    CHECK(ts.numBetsWinOneRoll.current == 2);
    CHECK(ts.numBetsWinOneRoll.max     == 2);

    CHECK(ts.amtBetsWinOneRoll.total   == 20);
    CHECK(ts.amtBetsWinOneRoll.current == 20);
    CHECK(ts.amtBetsWinOneRoll.max     == 20);
    CHECK(ts.maxAmtWinOneBet           == 10);

    CHECK(ts.numBetsOneRoll.total      == 2);
    CHECK(ts.numBetsOneRoll.current    == 2);
    CHECK(ts.numBetsOneRoll.max        == 2);

    CHECK(ts.amtBetsOneRoll.current    == 20);
    CHECK(ts.maxAmtBetOneBet           == 10);
    CHECK(ts.amtBetsOneRoll.max        == 20);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:betstats:win2")
{
    Gen::ErrorPass ep;
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;
    DecisionRecord dr1, dr2;

    // seq: 4,5,5,11,6,6,4

    CrapsBet p1b1("Player1", BetName::PassLine, 10);
    CrapsBet p2b1("Player2", BetName::PassLine, 10);
    point = 0; dice.set(2,2);
    CHECK(p1b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b1.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordKeep(p1b1);
    ts.recordKeep(p2b1);
    ts.recordDiceRoll(point, dice);

    CrapsBet p1b2("Player1", BetName::Come, 10);
    CrapsBet p2b2("Player2", BetName::Come, 10);
    point = 4; dice.set(2,3); dr1.reset(); dr2.reset();
    CHECK(p1b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b1.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    dr1.reset(); dr2.reset();
    CHECK(p1b2.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordKeep(p1b1);
    ts.recordKeep(p2b1);
    ts.recordKeep(p1b2);
    ts.recordKeep(p2b2);
    ts.recordDiceRoll(point, dice);

    point = 4; dice.set(2,3); dr1.reset(); dr2.reset();
    CHECK(p1b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b1.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    dr1.reset(); dr2.reset();
    CHECK(p1b2.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordKeep(p1b1);
    ts.recordKeep(p2b1);
    ts.recordWin(p1b2, p1b2.contractAmount() + p1b2.oddsAmount());
    ts.recordWin(p2b2, p2b2.contractAmount() + p2b2.oddsAmount());
    ts.recordDiceRoll(point, dice);

    CrapsBet p1b3("Player1", BetName::Come, 10);
    CrapsBet p2b3("Player2", BetName::Come, 10);
    point = 4; dice.set(5,6); dr1.reset(); dr2.reset();
    CHECK(p1b2.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    dr1.reset(); dr2.reset();
    CHECK(p1b3.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b3.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordKeep(p1b1);
    ts.recordKeep(p2b1);
    ts.recordWin(p1b3, p1b3.contractAmount() + p1b3.oddsAmount());
    ts.recordWin(p2b3, p2b3.contractAmount() + p2b3.oddsAmount());
    ts.recordDiceRoll(point, dice);

    CrapsBet p1b4("Player1", BetName::Come, 10);
    CrapsBet p2b4("Player2", BetName::Come, 10);
    point = 4; dice.set(3,3); dr1.reset(); dr2.reset();
    dr1.reset(); dr2.reset();
    CHECK(p1b3.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b3.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    dr1.reset(); dr2.reset();
    CHECK(p1b4.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b4.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordKeep(p1b1);
    ts.recordKeep(p2b1);
    ts.recordKeep(p1b4);
    ts.recordKeep(p2b4);
    ts.recordDiceRoll(point, dice);

    point = 4; dice.set(3,3); dr1.reset(); dr2.reset();
    CHECK(p1b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b1.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    dr1.reset(); dr2.reset();
    CHECK(p1b4.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b4.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordKeep(p1b1);
    ts.recordKeep(p2b1);
    ts.recordWin(p1b4, p1b4.contractAmount() + p1b4.oddsAmount());
    ts.recordWin(p2b4, p2b4.contractAmount() + p2b4.oddsAmount());
    ts.recordDiceRoll(point, dice);

    point = 4; dice.set(3,1); dr1.reset(); dr2.reset();
    CHECK(p1b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(p2b1.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordWin(p1b1, p1b1.contractAmount() + p1b1.oddsAmount());
    ts.recordWin(p2b1, p2b1.contractAmount() + p2b1.oddsAmount());
    ts.recordDiceRoll(point, dice);

    CHECK(ts.betsWinLose.lose["PassLine"].count == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amountBet == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amount == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].totDistance == 0);

    CHECK(ts.betsWinLose.wins["PassLine4"].count == 2);
    CHECK(ts.betsWinLose.wins["PassLine4"].amountBet == 20);
    CHECK(ts.betsWinLose.wins["PassLine4"].amount == 20);
    CHECK(ts.betsWinLose.wins["PassLine4"].totDistance == 10);

    CHECK(ts.betsWinLose.wins["Come5"].count == 2);
    CHECK(ts.betsWinLose.wins["Come5"].amountBet == 20);
    CHECK(ts.betsWinLose.wins["Come5"].amount == 20);
    CHECK(ts.betsWinLose.wins["Come5"].totDistance == 4);

    CHECK(ts.betsWinLose.wins["Come6"].count == 2);
    CHECK(ts.betsWinLose.wins["Come6"].amountBet == 20);
    CHECK(ts.betsWinLose.wins["Come6"].amount == 20);
    CHECK(ts.betsWinLose.wins["Come6"].totDistance == 4);

    CHECK(ts.totNumBetsAllBets         == 8);
    CHECK(ts.totAmtAllBets             == 80);

    CHECK(ts.numBetsWinOneRoll.total   == 8);
    CHECK(ts.numBetsWinOneRoll.current == 0);
    CHECK(ts.numBetsWinOneRoll.max     == 2);

    CHECK(ts.amtBetsWinOneRoll.total   == 80);
    CHECK(ts.amtBetsWinOneRoll.current == 0);
    CHECK(ts.amtBetsWinOneRoll.max     == 20);
    CHECK(ts.maxAmtWinOneBet           == 10);

    CHECK(ts.numBetsOneRoll.total      == 24);
    CHECK(ts.numBetsOneRoll.current    == 0);
    CHECK(ts.numBetsOneRoll.max        == 4);

    CHECK(ts.amtBetsOneRoll.current    == 0);
    CHECK(ts.maxAmtBetOneBet           == 10);
    CHECK(ts.amtBetsOneRoll.max        == 40);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:betstats:win3")
{
    Gen::ErrorPass ep;
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;
    DecisionRecord dr1, dr2;

    CrapsBet b1("Player1", BetName::Field, 10);
    CrapsBet b2("Player2", BetName::Field, 10);
    point = 0; dice.set(1,1);
    CHECK(b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    point = 0; dice.set(2,1);
    CHECK(b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordWin(b1, b1.contractAmount());
    ts.recordWin(b2, b2.contractAmount());

    CHECK(ts.betsWinLose.wins["Field2"].count       == 1);
    CHECK(ts.betsWinLose.wins["Field2"].amountBet   == 10);
    CHECK(ts.betsWinLose.wins["Field2"].amount      == 10);
    CHECK(ts.betsWinLose.wins["Field2"].totDistance == 1);

    CHECK(ts.betsWinLose.wins["Field3"].count       == 1);
    CHECK(ts.betsWinLose.wins["Field3"].amountBet   == 10);
    CHECK(ts.betsWinLose.wins["Field3"].amount      == 10);
    CHECK(ts.betsWinLose.wins["Field3"].totDistance == 1);

    CHECK(ts.totNumBetsAllBets         == 2);
    CHECK(ts.totAmtAllBets             == 20);

    CHECK(ts.numBetsWinOneRoll.total   == 2);
    CHECK(ts.numBetsWinOneRoll.current == 2);
    CHECK(ts.numBetsWinOneRoll.max     == 2);

    CHECK(ts.amtBetsWinOneRoll.total   == 20);
    CHECK(ts.amtBetsWinOneRoll.current == 20);
    CHECK(ts.amtBetsWinOneRoll.max     == 20);
    CHECK(ts.maxAmtWinOneBet           == 10);

    CHECK(ts.numBetsOneRoll.total      == 2);
    CHECK(ts.numBetsOneRoll.current    == 2);
    CHECK(ts.numBetsOneRoll.max        == 2);

    CHECK(ts.amtBetsOneRoll.current    == 20);
    CHECK(ts.maxAmtBetOneBet           == 10);
    CHECK(ts.amtBetsOneRoll.max        == 20);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:betstats:lose1")
{
    Gen::ErrorPass ep;
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;
    DecisionRecord dr1, dr2;

    CrapsBet b1("Player1", BetName::PassLine, 10);
    CrapsBet b2("Player2", BetName::PassLine, 10);
    point = 0; dice.set(2,1);
    CHECK(b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    point = 0; dice.set(6,6);
    CHECK(b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordLose(b1, b1.contractAmount());
    ts.recordLose(b2, b2.contractAmount());

    CHECK(ts.betsWinLose.wins["PassLine"].count       == 0);
    CHECK(ts.betsWinLose.wins["PassLine"].amountBet   == 0);
    CHECK(ts.betsWinLose.wins["PassLine"].amount      == 0);
    CHECK(ts.betsWinLose.wins["PassLine"].totDistance == 0);

    CHECK(ts.betsWinLose.lose["PassLine"].count       == 2);
    CHECK(ts.betsWinLose.lose["PassLine"].amountBet   == 20);
    CHECK(ts.betsWinLose.lose["PassLine"].amount      == 20);
    CHECK(ts.betsWinLose.lose["PassLine"].totDistance == 2);

    CHECK(ts.totNumBetsAllBets          == 2);
    CHECK(ts.totAmtAllBets              == 20);

    CHECK(ts.numBetsLoseOneRoll.total   == 2);
    CHECK(ts.numBetsLoseOneRoll.current == 2);
    CHECK(ts.numBetsLoseOneRoll.max     == 2);

    CHECK(ts.amtBetsLoseOneRoll.total   == 20);
    CHECK(ts.amtBetsLoseOneRoll.current == 20);
    CHECK(ts.amtBetsLoseOneRoll.max     == 20);
    CHECK(ts.maxAmtLoseOneBet           == 10);

    CHECK(ts.numBetsOneRoll.total       == 2);
    CHECK(ts.numBetsOneRoll.current     == 2);
    CHECK(ts.numBetsOneRoll.max         == 2);

    CHECK(ts.amtBetsOneRoll.current     == 20);
    CHECK(ts.maxAmtBetOneBet            == 10);
    CHECK(ts.amtBetsOneRoll.max         == 20);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:betstats:lose3")
{
    Gen::ErrorPass ep;
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;
    DecisionRecord dr1, dr2;

    CrapsBet b1("Player1", BetName::Field, 10);
    CrapsBet b2("Player2", BetName::Field, 10);
    point = 0; dice.set(3,3);
    CHECK(b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordLose(b1, b1.contractAmount());
    ts.recordLose(b2, b2.contractAmount());
    ts.recordDiceRoll(point, dice);

    CrapsBet b3("Player1", BetName::Field, 10);
    CrapsBet b4("Player2", BetName::Field, 10);
    point = 6; dice.set(4,4); dr1.reset(); dr2.reset();
    CHECK(b3.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(b4.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordLose(b3, b3.contractAmount());
    ts.recordLose(b4, b4.contractAmount());
    ts.recordDiceRoll(point, dice);

    CHECK(ts.betsWinLose.lose["Field6"].count       == 2);
    CHECK(ts.betsWinLose.lose["Field6"].amountBet   == 20);
    CHECK(ts.betsWinLose.lose["Field6"].amount      == 20);
    CHECK(ts.betsWinLose.lose["Field6"].totDistance == 2);

    CHECK(ts.betsWinLose.lose["Field8"].count       == 2);
    CHECK(ts.betsWinLose.lose["Field8"].amountBet   == 20);
    CHECK(ts.betsWinLose.lose["Field8"].amount      == 20);
    CHECK(ts.betsWinLose.lose["Field8"].totDistance == 2);

    CHECK(ts.totNumBetsAllBets          == 4);
    CHECK(ts.totAmtAllBets              == 40);

    CHECK(ts.numBetsLoseOneRoll.total   == 4);
    CHECK(ts.numBetsLoseOneRoll.current == 0);
    CHECK(ts.numBetsLoseOneRoll.max     == 2);

    CHECK(ts.amtBetsLoseOneRoll.total   == 40);
    CHECK(ts.amtBetsLoseOneRoll.current == 0);
    CHECK(ts.amtBetsLoseOneRoll.max     == 20);
    CHECK(ts.maxAmtLoseOneBet           == 10);

    CHECK(ts.numBetsOneRoll.total       == 4);
    CHECK(ts.numBetsOneRoll.current     == 0);
    CHECK(ts.numBetsOneRoll.max         == 2);

    CHECK(ts.amtBetsOneRoll.current     == 0);
    CHECK(ts.maxAmtBetOneBet            == 10);
    CHECK(ts.amtBetsOneRoll.max         == 20);
}

//----------------------------------------------------------------
