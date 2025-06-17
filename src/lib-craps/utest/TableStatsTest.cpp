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
initCheckNumberCounts(const TableStats::Counter& c)
{
    CHECK(c.count() == 0);
    CHECK(c.repeats() == 0);
    CHECK(c.armed == false);
}

void
initCheckPointCounts(const TableStats::PointCounts& pc)
{
    CHECK(pc.wins.count() == 0);
    CHECK(pc.wins.armed == false);
    CHECK(pc.wins.repeats() == 0);
    
    CHECK(pc.lose.count() == 0);
    CHECK(pc.lose.armed == false);
    CHECK(pc.lose.repeats() == 0);
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

    CHECK(c1.count() == 0);
    CHECK(c1.armed == false);
    CHECK(c1.repeats() == 0);

    c1.bump();
    CHECK(c1.count() == 1);
    CHECK(c1.armed == true);
    CHECK(c1.repeats() == 0);

    c1.bump();
    CHECK(c1.count() == 2);
    CHECK(c1.armed == true);
    CHECK(c1.repeats() == 1);

    c1.bump();
    CHECK(c1.count() == 3);
    CHECK(c1.armed == true);
    CHECK(c1.repeats() == 2);

    c1.disarm();
    CHECK(c1.count() == 3);
    CHECK(c1.armed == false);
    CHECK(c1.repeats() == 2);

    c1.bump();
    CHECK(c1.count() == 4);
    CHECK(c1.armed == true);
    CHECK(c1.repeats() == 2);

    c1.disarm();
    CHECK(c1.count() == 4);
    CHECK(c1.armed == false);
    CHECK(c1.repeats() == 2);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:one")
{
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;

    // Initial comeout roll, dice = 2, 
    // seq: 2
    point = 0; dice.set(1,1);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 1);
    CHECK(ts.numberCounts[2].count() == 1);
    CHECK(ts.numberCounts[2].repeats() == 0);
    CHECK(ts.comeOutRolls.count() == 1);
    CHECK(ts.comeOutRolls.repeats() == 0);
    CHECK(ts.pointRolls.count() == 0);
    CHECK(ts.pointRolls.repeats() == 0);
    CHECK(ts.passLineWins.count() == 0);
    CHECK(ts.passLineWins.repeats() == 0);
    CHECK(ts.passLineLose.count() == 1);
    CHECK(ts.passLineLose.repeats() == 0);
    CHECK(ts.dontPassWins.count() == 1);
    CHECK(ts.dontPassWins.repeats() == 0);
    CHECK(ts.dontPassLose.count() == 0);
    CHECK(ts.dontPassLose.repeats() == 0);
    CHECK(ts.comeWins.count() == 0);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 0);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.fieldBetWins.count() == 1);
    CHECK(ts.fieldBetWins.repeats() == 0);
    CHECK(ts.fieldBetLose.count() == 0);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 1);
    CHECK(ts.shooterCounts.repeats() == 0);
    CHECK(ts.twosOnComeOutRoll.count() == 1);
    CHECK(ts.twosOnComeOutRoll.repeats() == 0);
    CHECK(ts.threesOnComeOutRoll.count() == 0);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.count() == 0);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 1);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 0);

    // Same roll again, dice = 2, check for consecutives
    // seq: 2,2
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 2);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.comeOutRolls.count() == 2);
    CHECK(ts.comeOutRolls.repeats() == 1);
    CHECK(ts.pointRolls.count() == 0);
    CHECK(ts.pointRolls.repeats() == 0);
    CHECK(ts.passLineWins.count() == 0);
    CHECK(ts.passLineWins.repeats() == 0);
    CHECK(ts.passLineLose.count() == 2);
    CHECK(ts.passLineLose.repeats() == 1);
    CHECK(ts.dontPassWins.count() == 2);
    CHECK(ts.dontPassWins.repeats() == 1);
    CHECK(ts.dontPassLose.count() == 0);
    CHECK(ts.dontPassLose.repeats() == 0);
    CHECK(ts.comeWins.count() == 0);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 0);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.fieldBetWins.count() == 2);
    CHECK(ts.fieldBetWins.repeats() == 1);
    CHECK(ts.fieldBetLose.count() == 0);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 2);
    CHECK(ts.shooterCounts.repeats() == 1);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 0);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.count() == 0);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 2);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 1);

    // Still comeout roll, dice = 3, consecutive 2's stop
    // seq: 2,2,3
    point = 0; dice.set(1,2); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 3);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 1);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == true);
    CHECK(ts.comeOutRolls.count() == 3);
    CHECK(ts.comeOutRolls.repeats() == 2);
    CHECK(ts.pointRolls.count() == 0);
    CHECK(ts.pointRolls.repeats() == 0);
    CHECK(ts.passLineWins.count() == 0);
    CHECK(ts.passLineWins.repeats() == 0);
    CHECK(ts.passLineLose.count() == 3);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 0);
    CHECK(ts.dontPassLose.repeats() == 0);
    CHECK(ts.comeWins.count() == 0);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 0);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.fieldBetWins.count() == 3);
    CHECK(ts.fieldBetWins.repeats() == 2);
    CHECK(ts.fieldBetLose.count() == 0);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 3);
    CHECK(ts.shooterCounts.repeats() == 2);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.count() == 0);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 3);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 11, win on pass line
    // seq: 2,2,3,11
    point = 0; dice.set(5,6); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 4);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 1);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == true);
    CHECK(ts.comeOutRolls.count() == 4);
    CHECK(ts.comeOutRolls.repeats() == 3);
    CHECK(ts.pointRolls.count() == 0);
    CHECK(ts.pointRolls.repeats() == 0);
    CHECK(ts.passLineWins.count() == 1);
    CHECK(ts.passLineWins.repeats() == 0);
    CHECK(ts.passLineLose.count() == 3);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 1);
    CHECK(ts.dontPassLose.repeats() == 0);
    CHECK(ts.comeWins.count() == 0);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 0);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.fieldBetWins.count() == 4);
    CHECK(ts.fieldBetWins.repeats() == 3);
    CHECK(ts.fieldBetLose.count() == 0);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 4);
    CHECK(ts.shooterCounts.repeats() == 3);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 3);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 12, lose on pass line
    // seq: 2,2,3,11,12
    point = 0; dice.set(6,6); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 5);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 1);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == true);
    CHECK(ts.comeOutRolls.count() == 5);
    CHECK(ts.comeOutRolls.repeats() == 4);
    CHECK(ts.pointRolls.count() == 0);
    CHECK(ts.pointRolls.repeats() == 0);
    CHECK(ts.passLineWins.count() == 1);
    CHECK(ts.passLineWins.repeats() == 0);
    CHECK(ts.passLineLose.count() == 4);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 1);
    CHECK(ts.dontPassLose.repeats() == 0);
    CHECK(ts.comeWins.count() == 0);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 0);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.fieldBetWins.count() == 5);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 0);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 5);
    CHECK(ts.shooterCounts.repeats() == 4);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 7, win on pass line
    // seq: 2,2,3,11,12,7
    point = 0; dice.set(5,2); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 6);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 1);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[7].count() == 1);
    CHECK(ts.numberCounts[7].repeats() == 0);
    CHECK(ts.numberCounts[7].armed == true);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 6);
    CHECK(ts.comeOutRolls.repeats() == 5);
    CHECK(ts.pointRolls.count() == 0);
    CHECK(ts.pointRolls.repeats() == 0);
    CHECK(ts.passLineWins.count() == 2);
    CHECK(ts.passLineWins.repeats() == 0);
    CHECK(ts.passLineLose.count() == 4);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 2);
    CHECK(ts.dontPassLose.repeats() == 0);
    CHECK(ts.comeWins.count() == 0);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 0);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.fieldBetWins.count() == 5);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 1);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 6);
    CHECK(ts.shooterCounts.repeats() == 5);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.sevensOnComeOutRoll.count() == 1);
    CHECK(ts.sevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 7 again, win on pass line
    // seq: 2,2,3,11,12,7,7
    point = 0; dice.set(3,4); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 7);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 1);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[7].count() == 2);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == true);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 7);
    CHECK(ts.comeOutRolls.repeats() == 6);
    CHECK(ts.pointRolls.count() == 0);
    CHECK(ts.pointRolls.repeats() == 0);
    CHECK(ts.passLineWins.count() == 3);
    CHECK(ts.passLineWins.repeats() == 1);
    CHECK(ts.passLineLose.count() == 4);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 3);
    CHECK(ts.dontPassLose.repeats() == 1);
    CHECK(ts.comeWins.count() == 0);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 0);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.fieldBetWins.count() == 5);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 2);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 7);
    CHECK(ts.shooterCounts.repeats() == 6);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);

}

//----------------------------------------------------------------
