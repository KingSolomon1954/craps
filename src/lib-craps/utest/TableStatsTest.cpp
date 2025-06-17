//----------------------------------------------------------------
//
// File: TableStatsTest.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <doctest/doctest.h>
#include <gen/Timepoint.h>

using namespace Craps;

//----------------------------------------------------------------

void
initCheckNumberCounts(const TableStats::NumberCounts& nc)
{
    CHECK(nc.count == 0);
    CHECK(nc.curCnsectvCount == 0);
    CHECK(nc.maxCnsectvCount == 0);
}

void
initCheckPointCounts(const TableStats::PointCounts& pc)
{
    CHECK(pc.wins.count == 0);
    CHECK(pc.wins.armed == false);
    CHECK(pc.wins.curCnsectvCount == 0);
    CHECK(pc.wins.maxCnsectvCount == 0);
    
    CHECK(pc.lose.count == 0);
    CHECK(pc.lose.armed == false);
    CHECK(pc.lose.curCnsectvCount == 0);
    CHECK(pc.lose.maxCnsectvCount == 0);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:ctor")
{
    TableStats ts("tableId-LasVegas");
    CHECK(!ts.tableId.empty());
    Gen::Timepoint tp;
    CHECK(ts.sessionStart < tp);
    CHECK(ts.numRolls == 0);

    TableStats copy = ts;
    CHECK(copy.sessionStart == ts.sessionStart);

    for (unsigned i = 2; i < 13; i++)
    {
        initCheckNumberCounts(ts.numberCounts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.passLineCounts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.dontPassCounts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.comeCounts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.dontComeCounts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.hardwayCounts[i]);
    }
}

//----------------------------------------------------------------

TEST_CASE("TableStats:counter")
{
    TableStats::Counter c1;

    CHECK(c1.count == 0);
    CHECK(c1.armed == false);
    CHECK(c1.curCnsectvCount == 0);
    CHECK(c1.maxCnsectvCount == 0);

    c1.bump();
    CHECK(c1.count == 1);
    CHECK(c1.armed == true);
    CHECK(c1.curCnsectvCount == 0);
    CHECK(c1.maxCnsectvCount == 0);

    c1.bump();
    CHECK(c1.count == 2);
    CHECK(c1.armed == true);
    CHECK(c1.curCnsectvCount == 1);
    CHECK(c1.maxCnsectvCount == 1);

    c1.bump();
    CHECK(c1.count == 3);
    CHECK(c1.armed == true);
    CHECK(c1.curCnsectvCount == 2);
    CHECK(c1.maxCnsectvCount == 2);

    c1.disarm();
    CHECK(c1.count == 3);
    CHECK(c1.armed == false);
    CHECK(c1.curCnsectvCount == 0);
    CHECK(c1.maxCnsectvCount == 2);

    c1.bump();
    CHECK(c1.count == 4);
    CHECK(c1.armed == true);
    CHECK(c1.curCnsectvCount == 0);
    CHECK(c1.maxCnsectvCount == 2);

    c1.disarm();
    CHECK(c1.count == 4);
    CHECK(c1.armed == false);
    CHECK(c1.curCnsectvCount == 0);
    CHECK(c1.maxCnsectvCount == 2);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:one")
{
    TableStats ts("tableId-LasVegas");
    Dice prevRoll;
    Dice dice;
    unsigned point = 0;

    // Initial comeout roll, dice = 2, 
    point = 0; dice.set(1,1); prevRoll.set(6,6);
    ts.updateDiceRoll(point, dice, prevRoll);
    CHECK(ts.numRolls == 1);
    CHECK(ts.pointRolls.getCount() == 0);
    CHECK(ts.comeOutRolls.getCount() == 1);
    CHECK(ts.comeOutRolls.curCnsectvCount == 0);
    
    CHECK(ts.passLineWins.getCount() == 0);
    CHECK(ts.passLineLose.getCount() == 1);
    CHECK(ts.dontPassWins.getCount() == 1);
    CHECK(ts.dontPassLose.getCount() == 0);
    CHECK(ts.twosOnComeOutRoll.getCount() == 1);
    CHECK(ts.crapsOnComeOutRoll.getCount() == 1);

    // Same roll again, check for consecutives
    prevRoll = dice;
    ts.updateDiceRoll(point, dice, prevRoll);
    CHECK(ts.numRolls == 2);
    CHECK(ts.pointRolls.getCount() == 0);
    CHECK(ts.comeOutRolls.getCount() == 2);
    CHECK(ts.comeOutRolls.curCnsectvCount == 1);
    CHECK(ts.passLineWins.getCount() == 0);
    CHECK(ts.passLineLose.getCount() == 2);
    CHECK(ts.dontPassWins.getCount() == 2);
    CHECK(ts.dontPassLose.getCount() == 0);
    CHECK(ts.twosOnComeOutRoll.getCount() == 2);
    CHECK(ts.twosOnComeOutRoll.curCnsectvCount == 1);
    CHECK(ts.crapsOnComeOutRoll.getCount() == 2);
    CHECK(ts.crapsOnComeOutRoll.curCnsectvCount == 1);

    // Still comeout roll, dice = 3, 
    point = 0; prevRoll = dice; dice.set(1,2); 
    ts.updateDiceRoll(point, dice, prevRoll);
    CHECK(ts.numRolls == 3);
    CHECK(ts.pointRolls.getCount() == 0);
    CHECK(ts.comeOutRolls.getCount() == 3);
    CHECK(ts.comeOutRolls.curCnsectvCount == 2);
    CHECK(ts.passLineWins.getCount() == 0);
    CHECK(ts.passLineLose.getCount() == 3);
    CHECK(ts.dontPassWins.getCount() == 3);
    CHECK(ts.dontPassLose.getCount() == 0);
    CHECK(ts.twosOnComeOutRoll.getCount() == 2);
    CHECK(ts.twosOnComeOutRoll.curCnsectvCount == 0);
    CHECK(ts.threesOnComeOutRoll.getCount() == 1);
    CHECK(ts.threesOnComeOutRoll.curCnsectvCount == 0);
    CHECK(ts.crapsOnComeOutRoll.getCount() == 3);
    CHECK(ts.crapsOnComeOutRoll.curCnsectvCount == 2);
}

//----------------------------------------------------------------
