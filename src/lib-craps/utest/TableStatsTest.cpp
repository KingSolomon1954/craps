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
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);

    // Establish point, dice = 4
    // seq: 2,2,3,11,12,7,7,4
    point = 0; dice.set(2,2); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 8);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 1);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 1);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == true);
    CHECK(ts.numberCounts[7].count() == 2);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 8);
    CHECK(ts.comeOutRolls.repeats() == 7);
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
    CHECK(ts.fieldBetWins.count() == 6);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 2);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 8);
    CHECK(ts.shooterCounts.repeats() == 7);
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
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 0);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);

    // Point 4, roll an dice = 8
    // seq: 2,2,3,11,12,7,7,4,8
    point = 4; dice.set(4,4); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 9);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 1);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 1);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == false);
    CHECK(ts.numberCounts[7].count() == 2);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == false);
    CHECK(ts.numberCounts[8].count() == 1);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == true);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 8);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 1);
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
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == false);
    CHECK(ts.comeCounts[8].wins.count() == 0);
    CHECK(ts.comeCounts[8].wins.repeats() == 0);
    CHECK(ts.comeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].wins.count() == 0);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[4].wins.armed == false);
    CHECK(ts.dontComeCounts[8].wins.count() == 0);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.fieldBetWins.count() == 6);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 3);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 9);
    CHECK(ts.shooterCounts.repeats() == 8);
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
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);

    // Point 4, roll an dice = 3
    // seq: 2,2,3,11,12,7,7,4,8,3
    point = 4; dice.set(2,1); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 10);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 2);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == true);
    CHECK(ts.numberCounts[4].count() == 1);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == false);
    CHECK(ts.numberCounts[7].count() == 2);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == false);
    CHECK(ts.numberCounts[8].count() == 1);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 8);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 2);
    CHECK(ts.pointRolls.repeats() == 1);
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
    CHECK(ts.comeLose.count() == 1);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.dontComeWins.count() == 1);
    CHECK(ts.dontComeWins.repeats() == 0);
    CHECK(ts.dontComeLose.count() == 0);
    CHECK(ts.dontComeLose.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == false);
    CHECK(ts.comeCounts[8].wins.count() == 0);
    CHECK(ts.comeCounts[8].wins.repeats() == 0);
    CHECK(ts.comeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].wins.count() == 0);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[4].wins.armed == false);
    CHECK(ts.dontComeCounts[8].wins.count() == 0);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.fieldBetWins.count() == 7);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 3);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 10);
    CHECK(ts.shooterCounts.repeats() == 9);
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
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);

    // Point 4, roll an dice = 8
    // seq: 2,2,3,11,12,7,7,4,8,3,8
    point = 4; dice.set(5,3); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 11);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 2);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 1);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == false);
    CHECK(ts.numberCounts[7].count() == 2);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == false);
    CHECK(ts.numberCounts[8].count() == 2);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == true);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 8);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 3);
    CHECK(ts.pointRolls.repeats() == 2);
    CHECK(ts.passLineWins.count() == 3);
    CHECK(ts.passLineWins.repeats() == 1);
    CHECK(ts.passLineLose.count() == 4);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 3);
    CHECK(ts.dontPassLose.repeats() == 1);
    CHECK(ts.comeWins.count() == 1);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 1);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.dontComeWins.count() == 1);
    CHECK(ts.dontComeWins.repeats() == 0);
    CHECK(ts.dontComeLose.count() == 1);
    CHECK(ts.dontComeLose.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == false);
    CHECK(ts.comeCounts[8].wins.count() == 1);
    CHECK(ts.comeCounts[8].wins.repeats() == 1);
    CHECK(ts.comeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].wins.count() == 0);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[4].wins.armed == false);
    CHECK(ts.dontComeCounts[8].wins.count() == 0);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.fieldBetWins.count() == 7);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 4);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 11);
    CHECK(ts.shooterCounts.repeats() == 10);
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
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[4].lose.count() == 0);
    CHECK(ts.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);

    // Make the point 4, roll an dice = 4
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4
    point = 4; dice.set(3,1); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 12);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 2);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 2);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == true);
    CHECK(ts.numberCounts[7].count() == 2);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == false);
    CHECK(ts.numberCounts[8].count() == 2);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 8);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 4);
    CHECK(ts.pointRolls.repeats() == 3);
    CHECK(ts.passLineWins.count() == 4);
    CHECK(ts.passLineWins.repeats() == 2);
    CHECK(ts.passLineLose.count() == 4);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 4);
    CHECK(ts.dontPassLose.repeats() == 2);
    CHECK(ts.comeWins.count() == 1);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 1);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.dontComeWins.count() == 1);
    CHECK(ts.dontComeWins.repeats() == 0);
    CHECK(ts.dontComeLose.count() == 1);
    CHECK(ts.dontComeLose.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == true);
    CHECK(ts.comeCounts[8].wins.count() == 1);
    CHECK(ts.comeCounts[8].wins.repeats() == 1);
    CHECK(ts.comeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].wins.count() == 0);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[4].wins.armed == true);
    CHECK(ts.dontComeCounts[4].lose.count() == 0);
    CHECK(ts.dontComeCounts[4].lose.repeats() == 0);
    CHECK(ts.dontComeCounts[4].lose.armed == false);
    CHECK(ts.dontComeCounts[8].wins.count() == 0);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.fieldBetWins.count() == 8);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 4);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 12);
    CHECK(ts.shooterCounts.repeats() == 11);
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
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);

    // Come out roll, roll an dice = 12
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12
    point = 0; dice.set(6,6); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 13);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 2);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 2);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == false);
    CHECK(ts.numberCounts[7].count() == 2);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == false);
    CHECK(ts.numberCounts[8].count() == 2);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == true);
    CHECK(ts.comeOutRolls.count() == 9);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 4);
    CHECK(ts.pointRolls.repeats() == 3);
    CHECK(ts.passLineWins.count() == 4);
    CHECK(ts.passLineWins.repeats() == 2);
    CHECK(ts.passLineLose.count() == 5);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 4);
    CHECK(ts.dontPassLose.repeats() == 2);
    CHECK(ts.comeWins.count() == 1);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 1);
    CHECK(ts.comeLose.repeats() == 0);
    CHECK(ts.dontComeWins.count() == 1);
    CHECK(ts.dontComeWins.repeats() == 0);
    CHECK(ts.dontComeLose.count() == 1);
    CHECK(ts.dontComeLose.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == true);
    CHECK(ts.comeCounts[8].wins.count() == 1);
    CHECK(ts.comeCounts[8].wins.repeats() == 1);
    CHECK(ts.comeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].wins.count() == 0);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[4].wins.armed == true);
    CHECK(ts.dontComeCounts[8].wins.count() == 0);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 0);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.fieldBetWins.count() == 9);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 4);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 13);
    CHECK(ts.shooterCounts.repeats() == 12);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);

    // Come out roll, roll an dice = 7 lose come bets, win dont come bets
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12,7
    point = 0; dice.set(6,1); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 14);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 2);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 2);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == false);
    CHECK(ts.numberCounts[7].count() == 3);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == true);
    CHECK(ts.numberCounts[8].count() == 2);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 10);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 4);
    CHECK(ts.pointRolls.repeats() == 3);
    CHECK(ts.passLineWins.count() == 5);
    CHECK(ts.passLineWins.repeats() == 2);
    CHECK(ts.passLineLose.count() == 5);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 5);
    CHECK(ts.dontPassLose.repeats() == 2);
    CHECK(ts.comeWins.count() == 1);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 3);
    CHECK(ts.comeLose.repeats() == 1);
    CHECK(ts.dontComeWins.count() == 3);
    CHECK(ts.dontComeWins.repeats() == 1);
    CHECK(ts.dontComeLose.count() == 1);
    CHECK(ts.dontComeLose.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == false);
    CHECK(ts.comeCounts[8].wins.count() == 1);
    CHECK(ts.comeCounts[8].wins.repeats() == 1);
    CHECK(ts.comeCounts[8].wins.armed == false);
    CHECK(ts.comeCounts[4].lose.count() == 1);
    CHECK(ts.comeCounts[4].lose.repeats() == 0);
    CHECK(ts.comeCounts[4].lose.armed == false);
    CHECK(ts.comeCounts[8].lose.count() == 1);
    CHECK(ts.comeCounts[8].lose.repeats() == 0);
    CHECK(ts.comeCounts[8].lose.armed == false);
    CHECK(ts.dontComeCounts[4].wins.count() == 1);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 1);
    CHECK(ts.dontComeCounts[4].wins.armed == true);
    CHECK(ts.dontComeCounts[8].wins.count() == 1);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 1);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].lose.count() == 0);
    CHECK(ts.dontComeCounts[4].lose.repeats() == 0);
    CHECK(ts.dontComeCounts[4].lose.armed == false);
    CHECK(ts.dontComeCounts[8].lose.count() == 1);
    CHECK(ts.dontComeCounts[8].lose.repeats() == 0);
    CHECK(ts.dontComeCounts[8].lose.armed == false);
    CHECK(ts.fieldBetWins.count() == 9);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 5);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 14);
    CHECK(ts.shooterCounts.repeats() == 13);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.sevensOnComeOutRoll.count() == 3);
    CHECK(ts.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);

    // Establish point, roll an dice = 6
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12,7,6
    point = 0; dice.set(3,3); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 15);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 2);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 2);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == false);
    CHECK(ts.numberCounts[6].count() == 1);
    CHECK(ts.numberCounts[6].repeats() == 0);
    CHECK(ts.numberCounts[6].armed == true);
    CHECK(ts.numberCounts[7].count() == 3);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == false);
    CHECK(ts.numberCounts[8].count() == 2);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 11);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 4);
    CHECK(ts.pointRolls.repeats() == 3);
    CHECK(ts.passLineWins.count() == 5);
    CHECK(ts.passLineWins.repeats() == 2);
    CHECK(ts.passLineLose.count() == 5);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 3);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 5);
    CHECK(ts.dontPassLose.repeats() == 2);
    CHECK(ts.comeWins.count() == 1);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 3);
    CHECK(ts.comeLose.repeats() == 1);
    CHECK(ts.dontComeWins.count() == 3);
    CHECK(ts.dontComeWins.repeats() == 1);
    CHECK(ts.dontComeLose.count() == 1);
    CHECK(ts.dontComeLose.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == false);
    CHECK(ts.comeCounts[8].wins.count() == 1);
    CHECK(ts.comeCounts[8].wins.repeats() == 1);
    CHECK(ts.comeCounts[8].wins.armed == false);
    CHECK(ts.comeCounts[4].lose.count() == 1);
    CHECK(ts.comeCounts[4].lose.repeats() == 0);
    CHECK(ts.comeCounts[4].lose.armed == false);
    CHECK(ts.comeCounts[8].lose.count() == 1);
    CHECK(ts.comeCounts[8].lose.repeats() == 0);
    CHECK(ts.comeCounts[8].lose.armed == false);
    CHECK(ts.dontComeCounts[4].wins.count() == 1);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 1);
    CHECK(ts.dontComeCounts[4].wins.armed == true);
    CHECK(ts.dontComeCounts[8].wins.count() == 1);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 1);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].lose.count() == 0);
    CHECK(ts.dontComeCounts[4].lose.repeats() == 0);
    CHECK(ts.dontComeCounts[4].lose.armed == false);
    CHECK(ts.dontComeCounts[8].lose.count() == 1);
    CHECK(ts.dontComeCounts[8].lose.repeats() == 0);
    CHECK(ts.dontComeCounts[8].lose.armed == false);
    CHECK(ts.fieldBetWins.count() == 9);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 6);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 0);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 15);
    CHECK(ts.shooterCounts.repeats() == 14);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.sevensOnComeOutRoll.count() == 3);
    CHECK(ts.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);

    // Point 6, roll an dice = 7 seven out pass dice
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12,7,6,7
    point = 6; dice.set(3,4); 
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 16);
    CHECK(ts.numberCounts[2].count() == 2);
    CHECK(ts.numberCounts[2].repeats() == 1);
    CHECK(ts.numberCounts[2].armed == false);
    CHECK(ts.numberCounts[3].count() == 2);
    CHECK(ts.numberCounts[3].repeats() == 0);
    CHECK(ts.numberCounts[3].armed == false);
    CHECK(ts.numberCounts[4].count() == 2);
    CHECK(ts.numberCounts[4].repeats() == 0);
    CHECK(ts.numberCounts[4].armed == false);
    CHECK(ts.numberCounts[6].count() == 1);
    CHECK(ts.numberCounts[6].repeats() == 0);
    CHECK(ts.numberCounts[6].armed == false);
    CHECK(ts.numberCounts[7].count() == 4);
    CHECK(ts.numberCounts[7].repeats() == 1);
    CHECK(ts.numberCounts[7].armed == true);
    CHECK(ts.numberCounts[8].count() == 2);
    CHECK(ts.numberCounts[8].repeats() == 0);
    CHECK(ts.numberCounts[8].armed == false);
    CHECK(ts.numberCounts[11].count() == 1);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed == false);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed == false);
    CHECK(ts.comeOutRolls.count() == 11);
    CHECK(ts.comeOutRolls.repeats() == 7);
    CHECK(ts.pointRolls.count() == 5);
    CHECK(ts.pointRolls.repeats() == 3);
    CHECK(ts.passLineWins.count() == 5);
    CHECK(ts.passLineWins.repeats() == 2);
    CHECK(ts.passLineLose.count() == 6);
    CHECK(ts.passLineLose.repeats() == 2);
    CHECK(ts.dontPassWins.count() == 4);
    CHECK(ts.dontPassWins.repeats() == 2);
    CHECK(ts.dontPassLose.count() == 5);
    CHECK(ts.dontPassLose.repeats() == 2);
    CHECK(ts.comeWins.count() == 2);
    CHECK(ts.comeWins.repeats() == 0);
    CHECK(ts.comeLose.count() == 3);
    CHECK(ts.comeLose.repeats() == 1);
    CHECK(ts.dontComeWins.count() == 5);
    CHECK(ts.dontComeWins.repeats() == 1);
    CHECK(ts.dontComeLose.count() == 2);
    CHECK(ts.dontComeLose.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.count() == 0);
    CHECK(ts.comeCounts[4].wins.repeats() == 0);
    CHECK(ts.comeCounts[4].wins.armed == false);
    CHECK(ts.comeCounts[8].wins.count() == 1);
    CHECK(ts.comeCounts[8].wins.repeats() == 1);
    CHECK(ts.comeCounts[8].wins.armed == false);
    CHECK(ts.comeCounts[4].lose.count() == 1);
    CHECK(ts.comeCounts[4].lose.repeats() == 0);
    CHECK(ts.comeCounts[4].lose.armed == false);
    CHECK(ts.comeCounts[8].lose.count() == 1);
    CHECK(ts.comeCounts[8].lose.repeats() == 0);
    CHECK(ts.comeCounts[8].lose.armed == false);
    CHECK(ts.dontComeCounts[4].wins.count() == 1);
    CHECK(ts.dontComeCounts[4].wins.repeats() == 1);
    CHECK(ts.dontComeCounts[4].wins.armed == true);
    CHECK(ts.dontComeCounts[8].wins.count() == 1);
    CHECK(ts.dontComeCounts[8].wins.repeats() == 1);
    CHECK(ts.dontComeCounts[8].wins.armed == true);
    CHECK(ts.dontComeCounts[4].lose.count() == 0);
    CHECK(ts.dontComeCounts[4].lose.repeats() == 0);
    CHECK(ts.dontComeCounts[4].lose.armed == false);
    CHECK(ts.dontComeCounts[8].lose.count() == 1);
    CHECK(ts.dontComeCounts[8].lose.repeats() == 0);
    CHECK(ts.dontComeCounts[8].lose.armed == false);
    CHECK(ts.fieldBetWins.count() == 9);
    CHECK(ts.fieldBetWins.repeats() == 4);
    CHECK(ts.fieldBetLose.count() == 7);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.sevenOuts.count() == 1);
    CHECK(ts.sevenOuts.repeats() == 0);
    CHECK(ts.shooterCounts.count() == 16);
    CHECK(ts.shooterCounts.repeats() == 15);
    CHECK(ts.twosOnComeOutRoll.count() == 2);
    CHECK(ts.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.threesOnComeOutRoll.count() == 1);
    CHECK(ts.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.sevensOnComeOutRoll.count() == 3);
    CHECK(ts.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[4].lose.count() == 2);
    CHECK(ts.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[6].lose.count() == 1);
    CHECK(ts.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[8].lose.count() == 2);
    CHECK(ts.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.count() == 1);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);


    
}

//----------------------------------------------------------------

void
seqHelper(TableStats& ts, unsigned d1, unsigned d2)
{
    unsigned point = 0;
    Dice dice; dice.set(d1,d2);
    ts.recordDiceRoll(point, dice);
}

TEST_CASE("TableStats:recentrolls")
{
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;

    for (unsigned roll = 2; roll < 13; roll++)
    {
        unsigned d1; unsigned d2;
        switch(roll)
        {
            case 2:  d1 = 1; d2 = 1; break;
            case 3:  d1 = 1; d2 = 2; break;
            case 4:  d1 = 2; d2 = 2; break;
            case 5:  d1 = 2; d2 = 3; break;
            case 6:  d1 = 3; d2 = 3; break;
            case 7:  d1 = 3; d2 = 4; break;
            case 8:  d1 = 4; d2 = 4; break;
            case 9:  d1 = 5; d2 = 4; break;
            case 10: d1 = 6; d2 = 4; break;
            case 11: d1 = 6; d2 = 5; break;
            case 12: d1 = 6; d2 = 6; break;
        }
        seqHelper(ts, d1, d2);
    }

    unsigned expectedRoll = 2;
    for (const auto roll : ts.recentRolls)
    {
        CHECK(roll.value() == expectedRoll++);
    }
}

//----------------------------------------------------------------
