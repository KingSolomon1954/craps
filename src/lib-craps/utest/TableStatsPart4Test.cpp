//----------------------------------------------------------------
//
// File: TableStatsPart4Test.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <doctest/doctest.h>

using namespace Craps;

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:four")
{
    // seq 4,7,5,7
    
    TableStats ts;
    Dice dice;
    unsigned point = 0;

    // Initial comeout roll, dice = 4,
    // seq: 4
    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 1);
    CHECK(ts.rollStats.numberCounts[2].count()    == 0);
    CHECK(ts.rollStats.numberCounts[2].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[2].armed      == false);
    CHECK(ts.rollStats.numberCounts[3].count()    == 0);
    CHECK(ts.rollStats.numberCounts[3].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[3].armed      == false);
    CHECK(ts.rollStats.numberCounts[4].count()    == 1);
    CHECK(ts.rollStats.numberCounts[4].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[4].armed      == true);
    CHECK(ts.rollStats.numberCounts[5].count()    == 0);
    CHECK(ts.rollStats.numberCounts[5].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[5].armed      == false);
    CHECK(ts.rollStats.numberCounts[6].count()    == 0);
    CHECK(ts.rollStats.numberCounts[6].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[6].armed      == false);
    CHECK(ts.rollStats.numberCounts[7].count()    == 0);
    CHECK(ts.rollStats.numberCounts[7].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[7].armed      == false);
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

    CHECK(ts.rollStats.passWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.passWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.passWinsComeOut.armed          == false);
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

    CHECK(ts.rollStats.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.armed          == false);
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
    CHECK(ts.rollStats.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.comeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontComeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.armed      == false);
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

    CHECK(ts.rollStats.fieldBetWins.count()   == 1);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.armed     == true);
    CHECK(ts.rollStats.fieldBetLose.count()   == 0);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.fieldBetLose.armed     == false);

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
    CHECK(ts.rollStats.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.armed       == false);

    // Point roll, seven out dice = 7,
    // seq: 4,7
    point = 4; dice.set(5,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 2);
    CHECK(ts.rollStats.numberCounts[2].count()    == 0);
    CHECK(ts.rollStats.numberCounts[2].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[2].armed      == false);
    CHECK(ts.rollStats.numberCounts[3].count()    == 0);
    CHECK(ts.rollStats.numberCounts[3].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[3].armed      == false);
    CHECK(ts.rollStats.numberCounts[4].count()    == 1);
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
    CHECK(ts.rollStats.comeOutRolls.armed     == false);
    CHECK(ts.rollStats.pointRolls.count()     == 1);
    CHECK(ts.rollStats.pointRolls.repeats()   == 0);
    CHECK(ts.rollStats.pointRolls.armed       == true);

    CHECK(ts.rollStats.passWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.passWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.passWinsComeOut.armed          == false);
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
    CHECK(ts.rollStats.passPntCnts[4].lose.count()    == 1);
    CHECK(ts.rollStats.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[4].lose.armed      == true);
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
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.count()    == 1);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.armed      == true);
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

    CHECK(ts.rollStats.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.armed          == false);
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

    CHECK(ts.rollStats.comeWinsComeOut.count()        == 1);
    CHECK(ts.rollStats.comeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeWinsComeOut.armed          == true);
    CHECK(ts.rollStats.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.comeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontComeLoseComeOut.count()        == 1);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.armed          == true);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.armed      == false);
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

    CHECK(ts.rollStats.hardwayCounts[4].lose.count()    == 1);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count()    == 1);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count()    == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count()   == 1);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.armed     == true);

    CHECK(ts.rollStats.fieldBetWins.count()   == 1);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.armed     == false);
    CHECK(ts.rollStats.fieldBetLose.count()   == 1);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 0);
    CHECK(ts.rollStats.fieldBetLose.armed     == true);

    CHECK(ts.rollStats.sevenOuts.count()              == 1);
    CHECK(ts.rollStats.sevenOuts.repeats()            == 0);
    CHECK(ts.rollStats.sevenOuts.armed                == true);
    CHECK(ts.rollStats.shooterCounts.count()          == 0);
    CHECK(ts.rollStats.shooterCounts.repeats()        == 1);
    CHECK(ts.rollStats.shooterCounts.armed            == false);
    CHECK(ts.rollStats.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.armed        == false);
    CHECK(ts.rollStats.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.armed       == false);

    // Come out roll, dice = 5,
    // seq: 4,7,5
    point = 0; dice.set(3,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 3);
    CHECK(ts.rollStats.numberCounts[2].count()    == 0);
    CHECK(ts.rollStats.numberCounts[2].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[2].armed      == false);
    CHECK(ts.rollStats.numberCounts[3].count()    == 0);
    CHECK(ts.rollStats.numberCounts[3].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[3].armed      == false);
    CHECK(ts.rollStats.numberCounts[4].count()    == 1);
    CHECK(ts.rollStats.numberCounts[4].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[4].armed      == false);
    CHECK(ts.rollStats.numberCounts[5].count()    == 1);
    CHECK(ts.rollStats.numberCounts[5].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[5].armed      == true);
    CHECK(ts.rollStats.numberCounts[6].count()    == 0);
    CHECK(ts.rollStats.numberCounts[6].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[6].armed      == false);
    CHECK(ts.rollStats.numberCounts[7].count()    == 1);
    CHECK(ts.rollStats.numberCounts[7].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[7].armed      == false);
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
    CHECK(ts.rollStats.comeOutRolls.repeats() == 0);
    CHECK(ts.rollStats.comeOutRolls.armed     == true);
    CHECK(ts.rollStats.pointRolls.count()     == 1);
    CHECK(ts.rollStats.pointRolls.repeats()   == 0);
    CHECK(ts.rollStats.pointRolls.armed       == false);

    CHECK(ts.rollStats.passWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.passWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.passWinsComeOut.armed          == false);
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
    CHECK(ts.rollStats.passPntCnts[4].lose.count()    == 1);
    CHECK(ts.rollStats.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[4].lose.armed      == true);
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
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.count()    == 1);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.armed      == true);
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

    CHECK(ts.rollStats.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.armed          == false);
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

    CHECK(ts.rollStats.comeWinsComeOut.count()        == 1);
    CHECK(ts.rollStats.comeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeWinsComeOut.armed          == true);
    CHECK(ts.rollStats.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.comeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontComeLoseComeOut.count()        == 1);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.armed          == true);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.armed      == false);
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

    CHECK(ts.rollStats.hardwayCounts[4].lose.count()    == 1);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[4].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count()    == 1);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[6].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count()    == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.hardwayCounts[8].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count()   == 1);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.hardwayCounts[10].lose.armed     == true);

    CHECK(ts.rollStats.fieldBetWins.count()   == 1);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.armed     == false);
    CHECK(ts.rollStats.fieldBetLose.count()   == 2);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 1);
    CHECK(ts.rollStats.fieldBetLose.armed     == true);

    CHECK(ts.rollStats.sevenOuts.count()              == 1);
    CHECK(ts.rollStats.sevenOuts.repeats()            == 0);
    CHECK(ts.rollStats.sevenOuts.armed                == true);
    CHECK(ts.rollStats.shooterCounts.count()          == 1);
    CHECK(ts.rollStats.shooterCounts.repeats()        == 1);
    CHECK(ts.rollStats.shooterCounts.armed            == true);
    CHECK(ts.rollStats.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.armed        == false);
    CHECK(ts.rollStats.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.rollStats.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.rollStats.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.rollStats.crapsOnComeOutRoll.armed       == false);

    // Point roll, seven out dice = 7,
    // seq: 4,7,5,7
    point = 5; dice.set(4,3);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.rollStats.numRolls == 4);
    CHECK(ts.rollStats.numberCounts[2].count()    == 0);
    CHECK(ts.rollStats.numberCounts[2].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[2].armed      == false);
    CHECK(ts.rollStats.numberCounts[3].count()    == 0);
    CHECK(ts.rollStats.numberCounts[3].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[3].armed      == false);
    CHECK(ts.rollStats.numberCounts[4].count()    == 1);
    CHECK(ts.rollStats.numberCounts[4].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[4].armed      == false);
    CHECK(ts.rollStats.numberCounts[5].count()    == 1);
    CHECK(ts.rollStats.numberCounts[5].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[5].armed      == false);
    CHECK(ts.rollStats.numberCounts[6].count()    == 0);
    CHECK(ts.rollStats.numberCounts[6].repeats()  == 0);
    CHECK(ts.rollStats.numberCounts[6].armed      == false);
    CHECK(ts.rollStats.numberCounts[7].count()    == 2);
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

    CHECK(ts.rollStats.comeOutRolls.count()   == 2);
    CHECK(ts.rollStats.comeOutRolls.repeats() == 0);
    CHECK(ts.rollStats.comeOutRolls.armed     == false);
    CHECK(ts.rollStats.pointRolls.count()     == 2);
    CHECK(ts.rollStats.pointRolls.repeats()   == 0);
    CHECK(ts.rollStats.pointRolls.armed       == true);

    CHECK(ts.rollStats.passWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.passWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.passWinsComeOut.armed          == false);
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
    CHECK(ts.rollStats.passPntCnts[4].lose.count()    == 1);
    CHECK(ts.rollStats.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[4].lose.armed      == true);
    CHECK(ts.rollStats.passPntCnts[5].lose.count()    == 1);
    CHECK(ts.rollStats.passPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.passPntCnts[5].lose.armed      == true);
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
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.count()    == 1);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].wins.armed      == true);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.count()    == 1);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontPassPntCnts[5].wins.armed      == true);
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

    CHECK(ts.rollStats.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.armed          == false);
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

    CHECK(ts.rollStats.comeWinsComeOut.count()        == 2);
    CHECK(ts.rollStats.comeWinsComeOut.repeats()      == 1);
    CHECK(ts.rollStats.comeWinsComeOut.armed          == true);
    CHECK(ts.rollStats.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.comeLoseComeOut.count()        == 0);
    CHECK(ts.rollStats.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.rollStats.comeLoseComeOut.armed          == false);
    CHECK(ts.rollStats.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.comePntCnts[9].lose.armed      == false);
    CHECK(ts.rollStats.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.rollStats.comePntCnts[10].lose.armed     == false);

    CHECK(ts.rollStats.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.armed          == false);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.rollStats.dontComeLoseComeOut.count()        == 2);
    CHECK(ts.rollStats.dontComeLoseComeOut.repeats()      == 1);
    CHECK(ts.rollStats.dontComeLoseComeOut.armed          == true);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.armed      == false);
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

    CHECK(ts.rollStats.hardwayCounts[4].lose.count()    == 2);
    CHECK(ts.rollStats.hardwayCounts[4].lose.repeats()  == 1);
    CHECK(ts.rollStats.hardwayCounts[4].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[6].lose.count()    == 2);
    CHECK(ts.rollStats.hardwayCounts[6].lose.repeats()  == 1);
    CHECK(ts.rollStats.hardwayCounts[6].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[8].lose.count()    == 2);
    CHECK(ts.rollStats.hardwayCounts[8].lose.repeats()  == 1);
    CHECK(ts.rollStats.hardwayCounts[8].lose.armed      == true);
    CHECK(ts.rollStats.hardwayCounts[10].lose.count()   == 2);
    CHECK(ts.rollStats.hardwayCounts[10].lose.repeats() == 1);
    CHECK(ts.rollStats.hardwayCounts[10].lose.armed     == true);

    CHECK(ts.rollStats.fieldBetWins.count()   == 1);
    CHECK(ts.rollStats.fieldBetWins.repeats() == 0);
    CHECK(ts.rollStats.fieldBetWins.armed     == false);
    CHECK(ts.rollStats.fieldBetLose.count()   == 3);
    CHECK(ts.rollStats.fieldBetLose.repeats() == 2);
    CHECK(ts.rollStats.fieldBetLose.armed     == true);

    CHECK(ts.rollStats.sevenOuts.count()              == 2);
    CHECK(ts.rollStats.sevenOuts.repeats()            == 1);
    CHECK(ts.rollStats.sevenOuts.armed                == true);
    CHECK(ts.rollStats.shooterCounts.count()          == 0);
    CHECK(ts.rollStats.shooterCounts.repeats()        == 1);
    CHECK(ts.rollStats.shooterCounts.armed            == false);
    CHECK(ts.rollStats.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.rollStats.twosOnComeOutRoll.armed        == false);
    CHECK(ts.rollStats.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.threesOnComeOutRoll.armed      == false);
    CHECK(ts.rollStats.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.rollStats.sevensOnComeOutRoll.armed      == false);
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

TEST_CASE("TableStats:diceroll:five")
{
    // seq 4,5,5,6,6,8,8,9,9,10,10,4,7
    
    TableStats ts;
    Dice dice;
    unsigned point = 0;

    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(3,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(3,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(5,1);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(5,1);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(6,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(6,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(6,3);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(6,3);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(6,4);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(6,4);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 0; dice.set(3,4);
    ts.recordDiceRoll(point, dice);

    CHECK(ts.rollStats.numPointsEstablished == 1);
    CHECK(ts.rollStats.comeOutRolls.count() == 2);
    CHECK(ts.rollStats.pointRolls.count() == 11);
    CHECK(ts.rollStats.passWinsComeOut.count() == 1);
    CHECK(ts.rollStats.passLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 1);
    CHECK(ts.rollStats.passPntCnts[4].wins.count() == 1);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.count() == 1);
    
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);

    CHECK(ts.rollStats.comePntCnts[5].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[6].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[9].wins.count() == 1);
    CHECK(ts.rollStats.comePntCnts[10].wins.count() == 1);
    
    CHECK(ts.rollStats.comePntCnts[5].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[6].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[9].lose.count() == 1);
    CHECK(ts.rollStats.comePntCnts[10].lose.count() == 1);
    
    CHECK(ts.rollStats.dontComePntCnts[5].lose.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[6].lose.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].lose.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[9].lose.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[10].lose.count() == 1);

    CHECK(ts.rollStats.dontComePntCnts[5].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[6].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[8].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[9].wins.count() == 1);
    CHECK(ts.rollStats.dontComePntCnts[10].wins.count() == 1);

    CHECK(ts.rollStats.sevenOuts.count() == 0);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:six")
{
    // seq 4,4,4,4
    
    TableStats ts;
    Dice dice;
    unsigned point = 0;

    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(2,2);
    ts.recordDiceRoll(point, dice);

    CHECK(ts.rollStats.numPointsEstablished == 2);
    CHECK(ts.rollStats.comeOutRolls.count() == 2);
    CHECK(ts.rollStats.pointRolls.count() == 2);
    CHECK(ts.rollStats.passWinsComeOut.count() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.count() == 2);
    CHECK(ts.rollStats.passPntCnts[4].wins.repeats() == 1);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.count() == 2);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.repeats() == 1);
    
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:seven")
{
    // seq 4,5,5,5,5
    
    TableStats ts;
    Dice dice;
    unsigned point = 0;

    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(3,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(3,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(3,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(3,2);
    ts.recordDiceRoll(point, dice);

    CHECK(ts.rollStats.numPointsEstablished == 1);
    CHECK(ts.rollStats.comeOutRolls.count() == 1);
    CHECK(ts.rollStats.pointRolls.count() == 4);
    CHECK(ts.rollStats.passWinsComeOut.count() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.count() == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.repeats() == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.count() == 0);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.repeats() == 0);
    
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);

    CHECK(ts.rollStats.comePntCnts[5].wins.count() == 3);
    CHECK(ts.rollStats.comePntCnts[5].wins.repeats() == 2);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.count() == 3);
    CHECK(ts.rollStats.dontComePntCnts[5].lose.repeats() == 2);

}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:eight")
{
    // seq 4,4,4,4,4,4
    
    TableStats ts;
    Dice dice;
    unsigned point = 0;

    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    point = 4; dice.set(2,2);
    ts.recordDiceRoll(point, dice);

    CHECK(ts.rollStats.numPointsEstablished == 3);
    CHECK(ts.rollStats.comeOutRolls.count() == 3);
    CHECK(ts.rollStats.pointRolls.count() == 3);
    CHECK(ts.rollStats.passWinsComeOut.count() == 0);
    CHECK(ts.rollStats.passLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontPassLoseComeOut.count() == 0);
    CHECK(ts.rollStats.passPntCnts[4].wins.count() == 3);
    CHECK(ts.rollStats.passPntCnts[4].wins.repeats() == 2);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.count() == 3);
    CHECK(ts.rollStats.dontPassPntCnts[4].lose.repeats() == 2);
    
    CHECK(ts.rollStats.comeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.comeLoseComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeWinsComeOut.count() == 0);
    CHECK(ts.rollStats.dontComeLoseComeOut.count() == 0);

    CHECK(ts.rollStats.comePntCnts[4].wins.count() == 4);
    CHECK(ts.rollStats.comePntCnts[4].wins.repeats() == 3);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.count() == 4);
    CHECK(ts.rollStats.dontComePntCnts[4].lose.repeats() == 3);
}

//----------------------------------------------------------------
