//----------------------------------------------------------------
//
// File: TableStatsPart1Test.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <doctest/doctest.h>
#include <gen/Timepoint.h>

using namespace Craps;

//----------------------------------------------------------------

void
initCheckNumberCounts(const Counter& c)
{
    CHECK(c.count() == 0);
    CHECK(c.repeats() == 0);
    CHECK(c.armed == false);
}

void
initCheckPointCounts(const PointCounts& pc)
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
    CHECK(ts.rollStats.numRolls == 0);

    TableStats copy = ts;
    CHECK(copy.tableId == ts.tableId);

    for (unsigned i = 2; i < 13; i++)
    {
        initCheckNumberCounts(ts.rollStats.numberCounts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.rollStats.passPntCnts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.rollStats.dontPassPntCnts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.rollStats.comePntCnts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.rollStats.dontComePntCnts[i]);
    }
    for (unsigned i = 2; i < 11; i++)
    {
        initCheckPointCounts(ts.rollStats.hardwayCounts[i]);
    }
}

//----------------------------------------------------------------

TEST_CASE("TableStats:counter")
{
    Counter c1;

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

    Counter c2, c3;
    CHECK(c2 == c3);

    PointCounts pc1, pc2;
    CHECK(pc1 == pc2);

    BetStat bs1, bs2;
    CHECK(bs1 == bs2);
    
    NumBets nb1, nb2;
    CHECK(nb1 == nb2);

    AmtBets ab1, ab2;
    CHECK(ab1 == ab2);

    BetTypeStats bt1, bt2;
    CHECK(bt1 == bt2);

    BetStats bss1, bss2;
    CHECK(bss1 == bss2);

    RollStats rs1, rs2;
    CHECK(rs1 == rs2);

    TableStats ts1("id1");
    TableStats ts2("id2");
    CHECK(ts1 != ts2);  // session start time will be different
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

TEST_CASE("TableStats:diceroll:allsevens")
{
    // seq: 7,7
    
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;

    // Initial comeout roll, dice = 7,
    // seq: 7
    point = 0; dice.set(3,4);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 1);
    CHECK(ts.rollStats.numberCounts[2].count()    == 0);
    CHECK(ts.rollStats.numberCounts[2].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[2].armed      == false);
    CHECK(ts.rollStats.numberCounts[3].count()    == 0);
    CHECK(ts.rollStats.numberCounts[3].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[3].armed      == false);
    CHECK(ts.rollStats.numberCounts[4].count()    == 0);
    CHECK(ts.rollStats.numberCounts[4].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[4].armed      == false);
    CHECK(ts.rollStats.numberCounts[5].count()    == 0);
    CHECK(ts.rollStats.numberCounts[5].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[5].armed      == false);
    CHECK(ts.rollStats.numberCounts[6].count()    == 0);
    CHECK(ts.rollStats.numberCounts[6].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[6].armed      == false);
    CHECK(ts.rollStats.numberCounts[7].count()    == 1);
    CHECK(ts.rollStats.numberCounts[7].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[7].armed      == true);
    CHECK(ts.rollStats.numberCounts[8].count()    == 0);
    CHECK(ts.rollStats.numberCounts[8].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[8].armed      == false);
    CHECK(ts.rollStats.numberCounts[9].count()    == 0);
    CHECK(ts.rollStats.numberCounts[9].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[9].armed      == false);
    CHECK(ts.rollStats.numberCounts[10].count()   == 0);
    CHECK(ts.rollStats.numberCounts[10].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[10].armed     == false);
    CHECK(ts.rollStats.numberCounts[11].count()   == 0);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed     == false);
    CHECK(ts.rollStats.numberCounts[12].count()   == 0);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed     == false);

    CHECK(ts.rollStats.comeOutRolls.count()   == 1);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 0);
    CHECK(ts.rollStats.comeOutRolls.armed     == true);
    CHECK(ts.rollStats.pointRolls.count()     == 0);
    CHECK(ts.rollStats.pointRolls.repeats()   == 0);
    CHECK(ts.rollStats.pointRolls.armed       == false);

    CHECK(ts.rollStats.passWinsComeOut.count()        == 1);
    CHECK(ts.rollStats.passWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.passWinsComeOut.armed          == true);
    CHECK(ts.rollStats.passPntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.passPntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.passPntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.passLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.passLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.passLoseComeOut.armed          == false);
    CHECK(ts.rollStats.passPntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.passPntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.passPntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontPassWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontPassLoseComeOut.count()        == 1);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.armed          == true);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.comeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.comeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontComeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.hardwayCounts[4].wins.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count()   == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.armed     == false);

    CHECK(ts.rollStats.hardwayCounts[4].lose.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count()   == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.armed     == false);

    CHECK(ts.rollStats.fieldBetWins.count()   == 0);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.armed     == false);
    CHECK(ts.rollStats.fieldBetLose.count()   == 1);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.fieldBetLose.armed     == true);

    CHECK(ts.rollStats.sevenOuts.count()              == 0);
    CHECK(ts.rollStats.sevenOuts.repeats()            == 0);
    CHECK(ts.rollStats.sevenOuts.armed                == false);
    CHECK(ts.rollStats.shooterCounts.count()          == 1);
    CHECK(ts.rollStats.shooterCounts.repeats()        == 0);
    CHECK(ts.rollStats.shooterCounts.armed            == true);
    CHECK(ts.rollStats.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.armed        == false);
    CHECK(ts.rollStats.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count()    == 1);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.armed      == true);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.armed       == false);

    // Comeout roll, dice = 7,
    // seq: 7,7
    point = 0; dice.set(3,4);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 2);
    CHECK(ts.rollStats.numberCounts[2].count()    == 0);
    CHECK(ts.rollStats.numberCounts[2].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[2].armed      == false);
    CHECK(ts.rollStats.numberCounts[3].count()    == 0);
    CHECK(ts.rollStats.numberCounts[3].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[3].armed      == false);
    CHECK(ts.rollStats.numberCounts[4].count()    == 0);
    CHECK(ts.rollStats.numberCounts[4].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[4].armed      == false);
    CHECK(ts.rollStats.numberCounts[5].count()    == 0);
    CHECK(ts.rollStats.numberCounts[5].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[5].armed      == false);
    CHECK(ts.rollStats.numberCounts[6].count()    == 0);
    CHECK(ts.rollStats.numberCounts[6].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[6].armed      == false);
    CHECK(ts.rollStats.numberCounts[7].count()    == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats()  == 1);
    CHECK(ts.rollStats.numberCounts[7].armed      == true);
    CHECK(ts.rollStats.numberCounts[8].count()    == 0);
    CHECK(ts.rollStats.numberCounts[8].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[8].armed      == false);
    CHECK(ts.rollStats.numberCounts[9].count()    == 0);
    CHECK(ts.rollStats.numberCounts[9].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[9].armed      == false);
    CHECK(ts.rollStats.numberCounts[10].count()   == 0);
    CHECK(ts.rollStats.numberCounts[10].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[10].armed     == false);
    CHECK(ts.rollStats.numberCounts[11].count()   == 0);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed     == false);
    CHECK(ts.rollStats.numberCounts[12].count()   == 0);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed     == false);

    CHECK(ts.rollStats.comeOutRolls.count()   == 2);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 1);
    CHECK(ts.rollStats.comeOutRolls.armed     == true);
    CHECK(ts.rollStats.pointRolls.count()     == 0);
    CHECK(ts.rollStats.pointRolls.repeats()   == 0);
    CHECK(ts.rollStats.pointRolls.armed       == false);

    CHECK(ts.rollStats.passWinsComeOut.count()        == 2);
    CHECK(ts.rollStats.passWinsComeOut.repeats()      == 1);
    CHECK(ts.rollStats.passWinsComeOut.armed          == true);
    CHECK(ts.rollStats.passPntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.passPntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.passPntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.passPntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.passLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.passLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.passLoseComeOut.armed          == false);
    CHECK(ts.rollStats.passPntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.passPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.passPntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.passPntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.passPntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontPassWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontPassLoseComeOut.count()        == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats()      == 1);
    CHECK(ts.rollStats.dontPassLoseComeOut.armed          == true);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.dontPassPntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.dontPassPntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.comeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.comeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontComeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.hardwayCounts[4].wins.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count()   == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.armed     == false);

    CHECK(ts.rollStats.hardwayCounts[4].lose.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count()    == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.armed      == false);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count()   == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.armed     == false);

    CHECK(ts.rollStats.fieldBetWins.count()   == 0);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.armed     == false);
    CHECK(ts.rollStats.fieldBetLose.count()   == 2);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.fieldBetLose.armed     == true);

    CHECK(ts.rollStats.sevenOuts.count()              == 0);
    CHECK(ts.rollStats.sevenOuts.repeats()            == 0);
    CHECK(ts.rollStats.sevenOuts.armed                == false);
    CHECK(ts.rollStats.shooterCounts.count()          == 2);
    CHECK(ts.rollStats.shooterCounts.repeats()        == 1);
    CHECK(ts.rollStats.shooterCounts.armed            == true);
    CHECK(ts.rollStats.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.armed        == false);
    CHECK(ts.rollStats.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count()    == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats()  == 1);
    CHECK(ts.rollStats.sevensOnComeOutRoll.armed      == true);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.armed       == false);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:one")
{
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12,7,6,7
    
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;

    // Initial comeout roll, dice = 2,
    // seq: 2
    point = 0; dice.set(1,1);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 1);
    CHECK(ts.rollStats.numberCounts[2].count() == 1);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 0);
    CHECK(ts.rollStats.comeOutRolls.count() == 1);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 0);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 0);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 1);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 1);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 0);
    CHECK(ts.rollStats.fieldBetLose.count() == 0);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 1);
    CHECK(ts.rollStats.shooterCounts.repeats() == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 0);

    // Same roll again, dice = 2, check for consecutives
    // seq: 2,2
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 2);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.comeOutRolls.count() == 2);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 1);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 0);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 2);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 2);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 1);
    CHECK(ts.rollStats.fieldBetLose.count() == 0);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 2);
    CHECK(ts.rollStats.shooterCounts.repeats() == 1);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 1);

    // Still comeout roll, dice = 3, consecutive 2's stop
    // seq: 2,2,3
    point = 0; dice.set(1,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 3);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 1);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == true);
    CHECK(ts.rollStats.comeOutRolls.count() == 3);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 2);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 0);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 3);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 3);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 2);
    CHECK(ts.rollStats.fieldBetLose.count() == 0);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 3);
    CHECK(ts.rollStats.shooterCounts.repeats() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 3);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 11, win on pass line
    // seq: 2,2,3,11
    point = 0; dice.set(5,6);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 4);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 1);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == true);
    CHECK(ts.rollStats.comeOutRolls.count() == 4);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 3);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 1);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 3);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 1);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 4);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 3);
    CHECK(ts.rollStats.fieldBetLose.count() == 0);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 4);
    CHECK(ts.rollStats.shooterCounts.repeats() == 3);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 3);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 12, lose on pass line
    // seq: 2,2,3,11,12
    point = 0; dice.set(6,6);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 5);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 1);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == true);
    CHECK(ts.rollStats.comeOutRolls.count() == 5);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 4);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 1);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 1);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 5);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 0);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 5);
    CHECK(ts.rollStats.shooterCounts.repeats() == 4);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 7, win on pass line
    // seq: 2,2,3,11,12,7
    point = 0; dice.set(5,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 6);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 1);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 1);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[7].armed == true);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 6);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 5);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 2);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 5);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 1);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 6);
    CHECK(ts.rollStats.shooterCounts.repeats() == 5);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);

    // Still comeout roll, dice = 7 again, win on pass line
    // seq: 2,2,3,11,12,7,7
    point = 0; dice.set(3,4);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 7);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 1);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == true);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 7);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 6);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 5);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 2);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 7);
    CHECK(ts.rollStats.shooterCounts.repeats() == 6);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);

    // Establish point, dice = 4
    // seq: 2,2,3,11,12,7,7,4
    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 8);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 1);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 1);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == true);
    CHECK(ts.rollStats.numberCounts[7].count() == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 8);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 0);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.count() == 6);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 2);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 8);
    CHECK(ts.rollStats.shooterCounts.repeats() == 7);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);

    // Point 4, roll an dice = 8
    // seq: 2,2,3,11,12,7,7,4,8
    point = 4; dice.set(4,4);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 9);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 1);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 1);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == false);
    CHECK(ts.rollStats.numberCounts[8].count() == 1);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == true);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 8);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 1);
    CHECK(ts.rollStats.pointRolls.repeats() == 0);
    CHECK(ts.rollStats.passWinsComeOut.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 6);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 3);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 9);
    CHECK(ts.rollStats.shooterCounts.repeats() == 8);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);

    // Point 4, roll an dice = 3
    // seq: 2,2,3,11,12,7,7,4,8,3
    point = 4; dice.set(2,1);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 10);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 2);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == true);
    CHECK(ts.rollStats.numberCounts[4].count() == 1);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == false);
    CHECK(ts.rollStats.numberCounts[8].count() == 1);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 8);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 2);
    CHECK(ts.rollStats.pointRolls.repeats() == 1);
    CHECK(ts.rollStats.passWinsComeOut.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 7);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 3);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 10);
    CHECK(ts.rollStats.shooterCounts.repeats() == 9);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);

    // Point 4, roll an dice = 8
    // seq: 2,2,3,11,12,7,7,4,8,3,8
    point = 4; dice.set(5,3);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 11);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 2);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 1);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == false);
    CHECK(ts.rollStats.numberCounts[8].count() == 2);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == true);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 8);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 3);
    CHECK(ts.rollStats.pointRolls.repeats() == 2);
    CHECK(ts.rollStats.passWinsComeOut.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 7);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 4);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 11);
    CHECK(ts.rollStats.shooterCounts.repeats() == 10);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);

    // Made the point 4, roll an dice = 4
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4
    point = 4; dice.set(3,1);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 12);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 2);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 2);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == true);
    CHECK(ts.rollStats.numberCounts[7].count() == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == false);
    CHECK(ts.rollStats.numberCounts[8].count() == 2);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 8);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 4);
    CHECK(ts.rollStats.pointRolls.repeats() == 3);
    CHECK(ts.rollStats.passWinsComeOut.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 4);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 8);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 4);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 12);
    CHECK(ts.rollStats.shooterCounts.repeats() == 11);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 4);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);

    // Come out roll, roll an dice = 12
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12
    point = 0; dice.set(6,6);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 13);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 2);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 2);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 2);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == false);
    CHECK(ts.rollStats.numberCounts[8].count() == 2);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == true);
    CHECK(ts.rollStats.comeOutRolls.count() == 9);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 4);
    CHECK(ts.rollStats.pointRolls.repeats() == 3);
    CHECK(ts.rollStats.passWinsComeOut.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 5);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 9);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 4);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 13);
    CHECK(ts.rollStats.shooterCounts.repeats() == 12);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);

    // Come out roll, roll an dice = 7 lose come bets, win dont come bets
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12,7
    point = 0; dice.set(6,1);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 14);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 2);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 2);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 3);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == true);
    CHECK(ts.rollStats.numberCounts[8].count() == 2);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 10);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 4);
    CHECK(ts.rollStats.pointRolls.repeats() == 3);
    CHECK(ts.rollStats.passWinsComeOut.count() == 4);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 5);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 4);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed == true);
    CHECK(ts.rollStats.comePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 9);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 5);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 14);
    CHECK(ts.rollStats.shooterCounts.repeats() == 13);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 3);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);

    // Establish point, roll an dice = 6
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12,7,6
    point = 0; dice.set(3,3);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 15);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 2);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 2);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == false);
    CHECK(ts.rollStats.numberCounts[6].count() == 1);
    CHECK(ts.rollStats.numberCounts[6].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[6].armed == true);
    CHECK(ts.rollStats.numberCounts[7].count() == 3);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == false);
    CHECK(ts.rollStats.numberCounts[8].count() == 2);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 11);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 4);
    CHECK(ts.rollStats.pointRolls.repeats() == 3);
    CHECK(ts.rollStats.passWinsComeOut.count() == 4);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 5);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 4);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed == true);
    CHECK(ts.rollStats.comePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 9);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 6);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.sevenOuts.count() == 0);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 15);
    CHECK(ts.rollStats.shooterCounts.repeats() == 14);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 3);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);

    // Point 6, roll an dice = 7 seven out pass dice
    // seq: 2,2,3,11,12,7,7,4,8,3,8,4,12,7,6,7
    point = 6; dice.set(3,4);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 16);
    CHECK(ts.rollStats.numberCounts[2].count() == 2);
    CHECK(ts.rollStats.numberCounts[2].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[2].armed == false);
    CHECK(ts.rollStats.numberCounts[3].count() == 2);
    CHECK(ts.rollStats.numberCounts[3].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[3].armed == false);
    CHECK(ts.rollStats.numberCounts[4].count() == 2);
    CHECK(ts.rollStats.numberCounts[4].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[4].armed == false);
    CHECK(ts.rollStats.numberCounts[6].count() == 1);
    CHECK(ts.rollStats.numberCounts[6].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[6].armed == false);
    CHECK(ts.rollStats.numberCounts[7].count() == 4);
    CHECK(ts.rollStats.numberCounts[7].repeats() == 1);
    CHECK(ts.rollStats.numberCounts[7].armed == true);
    CHECK(ts.rollStats.numberCounts[8].count() == 2);
    CHECK(ts.rollStats.numberCounts[8].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[8].armed == false);
    CHECK(ts.rollStats.numberCounts[11].count() == 1);
    CHECK(ts.rollStats.numberCounts[11].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[11].armed == false);
    CHECK(ts.rollStats.numberCounts[12].repeats() == 0);
    CHECK(ts.rollStats.numberCounts[12].armed == false);
    CHECK(ts.rollStats.comeOutRolls.count() == 11);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 7);
    CHECK(ts.rollStats.pointRolls.count() == 5);
    CHECK(ts.rollStats.pointRolls.repeats() == 3);
    CHECK(ts.rollStats.passWinsComeOut.count() == 4);
    CHECK(ts.rollStats.passWinsComeOut.repeats() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 5);
    CHECK(ts.rollStats.passLoseComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 3);
    CHECK(ts.rollStats.dontPassWinsComeOut.repeats() == 2);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 4);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.comeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.comeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 1);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed == true);
    CHECK(ts.rollStats.comePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed == true);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed == false);
    CHECK(ts.rollStats.fieldBetWins.count() == 9);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 4);
    CHECK(ts.rollStats.fieldBetLose.count() == 7);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 2);
    CHECK(ts.rollStats.sevenOuts.count() == 1);
    CHECK(ts.rollStats.sevenOuts.repeats() == 0);
    CHECK(ts.rollStats.shooterCounts.count() == 0);
    CHECK(ts.rollStats.shooterCounts.repeats() == 15);
    CHECK(ts.rollStats.twosOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count() == 3);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count() == 1);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count() == 2);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count() == 5);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[6].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].wins.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[8].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.count() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.count() == 2);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats() == 1);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count() == 2);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats() == 1);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count() == 1);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);
}

//----------------------------------------------------------------
