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

void initCheckNumberCounts(const TableStats::NumberCounts& nc)
{
    CHECK(nc.count == 0);
    CHECK(nc.curCnsectvCount == 0);
    CHECK(nc.maxCnsectvCount == 0);
}

void initCheckPointCounts(const TableStats::PointCounts& pc)
{
    CHECK(pc.numWins == 0);
    CHECK(pc.numLose == 0);
    CHECK(pc.armed == false);
    CHECK(pc.curCnsectvWinsCount == 0);
    CHECK(pc.maxCnsectvWinsCount == 0);
    CHECK(pc.curCnsectvLoseCount == 0);
    CHECK(pc.maxCnsectvLoseCount == 0);
}

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

TEST_CASE("TableStats:diceroll:one")
{
    TableStats ts("tableId-LasVegas");
    Dice prevRoll;
    Dice dice;
    unsigned point = 0;

    // Come out roll, dice = 2, 
    point = 0; dice.set(1,1); prevRoll.set(6,6);
    ts.updateDiceRoll(point, dice, prevRoll);
    CHECK(ts.numRolls == 1);
    CHECK(ts.comeOutRolls.getCount() == 1);
    CHECK(ts.comeOutRolls.curCnsectvCount == 0);
    CHECK(ts.pointRolls.getCount() == 0);
}

//----------------------------------------------------------------
