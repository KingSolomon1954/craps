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
    ts.recordWin(&b1, b1.contractAmount());
    ts.recordWin(&b2, b2.contractAmount());

    CHECK(ts.betsWinLose.wins["PassLine"].count == 2);
    CHECK(ts.betsWinLose.wins["PassLine"].amountBet == 20);
    CHECK(ts.betsWinLose.wins["PassLine"].amount == 20);
    CHECK(ts.betsWinLose.wins["PassLine"].totDistance == 2);
    
    CHECK(ts.betsWinLose.lose["PassLine"].count == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amountBet == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amount == 0);
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
    
    CrapsBet b1("Player1", BetName::PassLine, 10);
    CrapsBet b2("Player2", BetName::PassLine, 10);
    point = 0; dice.set(3,4);
    CHECK(b1.evaluate(point, dice, dr1, ep) == Gen::ReturnCode::Success);
    CHECK(b2.evaluate(point, dice, dr2, ep) == Gen::ReturnCode::Success);
    ts.recordWin(&b1, b1.contractAmount());
    ts.recordWin(&b2, b2.contractAmount());

    CHECK(ts.betsWinLose.wins["PassLine"].count == 2);
    CHECK(ts.betsWinLose.wins["PassLine"].amountBet == 20);
    CHECK(ts.betsWinLose.wins["PassLine"].amount == 20);
    CHECK(ts.betsWinLose.wins["PassLine"].totDistance == 2);
    
    CHECK(ts.betsWinLose.lose["PassLine"].count == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amountBet == 0);
    CHECK(ts.betsWinLose.lose["PassLine"].amount == 0);
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
    ts.recordWin(&b1, b1.contractAmount());
    ts.recordWin(&b2, b2.contractAmount());

    CHECK(ts.betsWinLose.wins["Field2"].count == 1);
    CHECK(ts.betsWinLose.wins["Field2"].amountBet == 10);
    CHECK(ts.betsWinLose.wins["Field2"].amount == 10);
    CHECK(ts.betsWinLose.wins["Field2"].totDistance == 1);
    
    CHECK(ts.betsWinLose.wins["Field3"].count == 1);
    CHECK(ts.betsWinLose.wins["Field3"].amountBet == 10);
    CHECK(ts.betsWinLose.wins["Field3"].amount == 10);
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
    ts.recordLose(&b1, b1.contractAmount());
    ts.recordLose(&b2, b2.contractAmount());

    CHECK(ts.betsWinLose.wins["PassLine"].count == 0);
    CHECK(ts.betsWinLose.wins["PassLine"].amountBet == 0);
    CHECK(ts.betsWinLose.wins["PassLine"].amount == 0);
    CHECK(ts.betsWinLose.wins["PassLine"].totDistance == 0);
    
    CHECK(ts.betsWinLose.lose["PassLine"].count == 2);
    CHECK(ts.betsWinLose.lose["PassLine"].amountBet == 20);
    CHECK(ts.betsWinLose.lose["PassLine"].amount == 20);
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
