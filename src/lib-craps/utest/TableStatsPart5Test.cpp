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
    DecisionRecord dr;
    
    CrapsBet b("Player1", BetName::PassLine, 100);
    point = 0; dice.set(3,4);
    CHECK(b.evaluate(point, dice, dr, ep) == Gen::ReturnCode::Success);
    ts.recordWin(&b, b.contractAmount());
    CHECK(ts.totNumBetsAllBets == 1);
    CHECK(ts.totAmtAllBets == 100);
}

//----------------------------------------------------------------
