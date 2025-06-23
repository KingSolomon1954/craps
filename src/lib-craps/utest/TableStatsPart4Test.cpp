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
    
    TableStats ts("tableId-LasVegas");
    Dice dice;
    unsigned point = 0;

    // Initial comeout roll, dice = 4,
    // seq: 4
    point = 0; dice.set(2,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 1);
    CHECK(ts.numberCounts[2].count()    == 0);
    CHECK(ts.numberCounts[2].repeats()  == 0);
    CHECK(ts.numberCounts[2].armed      == false);
    CHECK(ts.numberCounts[3].count()    == 0);
    CHECK(ts.numberCounts[3].repeats()  == 0);
    CHECK(ts.numberCounts[3].armed      == false);
    CHECK(ts.numberCounts[4].count()    == 1);
    CHECK(ts.numberCounts[4].repeats()  == 0);
    CHECK(ts.numberCounts[4].armed      == true);
    CHECK(ts.numberCounts[5].count()    == 0);
    CHECK(ts.numberCounts[5].repeats()  == 0);
    CHECK(ts.numberCounts[5].armed      == false);
    CHECK(ts.numberCounts[6].count()    == 0);
    CHECK(ts.numberCounts[6].repeats()  == 0);
    CHECK(ts.numberCounts[6].armed      == false);
    CHECK(ts.numberCounts[7].count()    == 0);
    CHECK(ts.numberCounts[7].repeats()  == 0);
    CHECK(ts.numberCounts[7].armed      == false);
    CHECK(ts.numberCounts[8].count()    == 0);
    CHECK(ts.numberCounts[8].repeats()  == 0);
    CHECK(ts.numberCounts[8].armed      == false);
    CHECK(ts.numberCounts[9].count()    == 0);
    CHECK(ts.numberCounts[9].repeats()  == 0);
    CHECK(ts.numberCounts[9].armed      == false);
    CHECK(ts.numberCounts[10].count()   == 0);
    CHECK(ts.numberCounts[10].repeats() == 0);
    CHECK(ts.numberCounts[10].armed     == false);
    CHECK(ts.numberCounts[11].count()   == 0);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed     == false);
    CHECK(ts.numberCounts[12].count()   == 0);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed     == false);

    CHECK(ts.comeOutRolls.count()   == 1);
    CHECK(ts.comeOutRolls.repeats() == 0);
    CHECK(ts.comeOutRolls.armed     == true);
    CHECK(ts.pointRolls.count()     == 0);
    CHECK(ts.pointRolls.repeats()   == 0);
    CHECK(ts.pointRolls.armed       == false);

    CHECK(ts.passWinsComeOut.count()        == 0);
    CHECK(ts.passWinsComeOut.repeats()      == 0);
    CHECK(ts.passWinsComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].wins.count()    == 0);
    CHECK(ts.passPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[4].wins.armed      == false);
    CHECK(ts.passPntCnts[5].wins.count()    == 0);
    CHECK(ts.passPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[5].wins.armed      == false);
    CHECK(ts.passPntCnts[6].wins.count()    == 0);
    CHECK(ts.passPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[6].wins.armed      == false);
    CHECK(ts.passPntCnts[8].wins.count()    == 0);
    CHECK(ts.passPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[8].wins.armed      == false);
    CHECK(ts.passPntCnts[9].wins.count()    == 0);
    CHECK(ts.passPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[9].wins.armed      == false);
    CHECK(ts.passPntCnts[10].wins.count()   == 0);
    CHECK(ts.passPntCnts[10].wins.repeats() == 0);
    CHECK(ts.passPntCnts[10].wins.armed     == false);

    CHECK(ts.passLoseComeOut.count()        == 0);
    CHECK(ts.passLoseComeOut.repeats()      == 0);
    CHECK(ts.passLoseComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].lose.count()    == 0);
    CHECK(ts.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[4].lose.armed      == false);
    CHECK(ts.passPntCnts[5].lose.count()    == 0);
    CHECK(ts.passPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[5].lose.armed      == false);
    CHECK(ts.passPntCnts[6].lose.count()    == 0);
    CHECK(ts.passPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[6].lose.armed      == false);
    CHECK(ts.passPntCnts[8].lose.count()    == 0);
    CHECK(ts.passPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[8].lose.armed      == false);
    CHECK(ts.passPntCnts[9].lose.count()    == 0);
    CHECK(ts.passPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[9].lose.armed      == false);
    CHECK(ts.passPntCnts[10].lose.count()   == 0);
    CHECK(ts.passPntCnts[10].lose.repeats() == 0);
    CHECK(ts.passPntCnts[10].lose.armed     == false);

    CHECK(ts.dontPassWinsComeOut.count()        == 0);
    CHECK(ts.dontPassWinsComeOut.repeats()      == 0);
    CHECK(ts.dontPassWinsComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[5].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[6].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[8].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[9].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[10].wins.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].wins.armed     == false);

    CHECK(ts.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.dontPassLoseComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[5].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[6].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[8].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[9].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[10].lose.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].lose.armed     == false);

    CHECK(ts.comeWinsComeOut.count()        == 0);
    CHECK(ts.comeWinsComeOut.repeats()      == 0);
    CHECK(ts.comeWinsComeOut.armed          == false);
    CHECK(ts.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[4].wins.armed      == false);
    CHECK(ts.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[5].wins.armed      == false);
    CHECK(ts.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[6].wins.armed      == false);
    CHECK(ts.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[8].wins.armed      == false);
    CHECK(ts.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[9].wins.armed      == false);
    CHECK(ts.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.comePntCnts[10].wins.armed     == false);

    CHECK(ts.comeLoseComeOut.count()        == 0);
    CHECK(ts.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.comeLoseComeOut.armed          == false);
    CHECK(ts.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[4].lose.armed      == false);
    CHECK(ts.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[5].lose.armed      == false);
    CHECK(ts.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[6].lose.armed      == false);
    CHECK(ts.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[8].lose.armed      == false);
    CHECK(ts.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[9].lose.armed      == false);
    CHECK(ts.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.comePntCnts[10].lose.armed     == false);

    CHECK(ts.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.dontComeWinsComeOut.armed          == false);
    CHECK(ts.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.dontComeLoseComeOut.count()        == 0);
    CHECK(ts.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.dontComeLoseComeOut.armed          == false);
    CHECK(ts.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].lose.armed      == false);
    CHECK(ts.dontComePntCnts[10].lose.count()   == 0);
    CHECK(ts.dontComePntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].lose.armed     == false);

    CHECK(ts.hardwayCounts[4].wins.count()    == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[4].wins.armed      == false);
    CHECK(ts.hardwayCounts[6].wins.count()    == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[6].wins.armed      == false);
    CHECK(ts.hardwayCounts[8].wins.count()    == 0);
    CHECK(ts.hardwayCounts[8].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[8].wins.armed      == false);
    CHECK(ts.hardwayCounts[10].wins.count()   == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.armed     == false);

    CHECK(ts.hardwayCounts[4].lose.count()    == 0);
    CHECK(ts.hardwayCounts[4].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[4].lose.armed      == false);
    CHECK(ts.hardwayCounts[6].lose.count()    == 0);
    CHECK(ts.hardwayCounts[6].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[6].lose.armed      == false);
    CHECK(ts.hardwayCounts[8].lose.count()    == 0);
    CHECK(ts.hardwayCounts[8].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[8].lose.armed      == false);
    CHECK(ts.hardwayCounts[10].lose.count()   == 0);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.armed     == false);

    CHECK(ts.fieldBetWins.count()   == 1);
    CHECK(ts.fieldBetWins.repeats() == 0);
    CHECK(ts.fieldBetWins.armed     == true);
    CHECK(ts.fieldBetLose.count()   == 0);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.fieldBetLose.armed     == false);

    CHECK(ts.sevenOuts.count()              == 0);
    CHECK(ts.sevenOuts.repeats()            == 0);
    CHECK(ts.sevenOuts.armed                == false);
    CHECK(ts.shooterCounts.count()          == 1);
    CHECK(ts.shooterCounts.repeats()        == 0);
    CHECK(ts.shooterCounts.armed            == true);
    CHECK(ts.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.twosOnComeOutRoll.armed        == false);
    CHECK(ts.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.threesOnComeOutRoll.armed      == false);
    CHECK(ts.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.sevensOnComeOutRoll.armed      == false);
    CHECK(ts.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.crapsOnComeOutRoll.armed       == false);

    // Point roll, seven out dice = 7,
    // seq: 4,7
    point = 4; dice.set(5,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 2);
    CHECK(ts.numberCounts[2].count()    == 0);
    CHECK(ts.numberCounts[2].repeats()  == 0);
    CHECK(ts.numberCounts[2].armed      == false);
    CHECK(ts.numberCounts[3].count()    == 0);
    CHECK(ts.numberCounts[3].repeats()  == 0);
    CHECK(ts.numberCounts[3].armed      == false);
    CHECK(ts.numberCounts[4].count()    == 1);
    CHECK(ts.numberCounts[4].repeats()  == 0);
    CHECK(ts.numberCounts[4].armed      == false);
    CHECK(ts.numberCounts[5].count()    == 0);
    CHECK(ts.numberCounts[5].repeats()  == 0);
    CHECK(ts.numberCounts[5].armed      == false);
    CHECK(ts.numberCounts[6].count()    == 0);
    CHECK(ts.numberCounts[6].repeats()  == 0);
    CHECK(ts.numberCounts[6].armed      == false);
    CHECK(ts.numberCounts[7].count()    == 1);
    CHECK(ts.numberCounts[7].repeats()  == 0);
    CHECK(ts.numberCounts[7].armed      == true);
    CHECK(ts.numberCounts[8].count()    == 0);
    CHECK(ts.numberCounts[8].repeats()  == 0);
    CHECK(ts.numberCounts[8].armed      == false);
    CHECK(ts.numberCounts[9].count()    == 0);
    CHECK(ts.numberCounts[9].repeats()  == 0);
    CHECK(ts.numberCounts[9].armed      == false);
    CHECK(ts.numberCounts[10].count()   == 0);
    CHECK(ts.numberCounts[10].repeats() == 0);
    CHECK(ts.numberCounts[10].armed     == false);
    CHECK(ts.numberCounts[11].count()   == 0);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed     == false);
    CHECK(ts.numberCounts[12].count()   == 0);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed     == false);

    CHECK(ts.comeOutRolls.count()   == 1);
    CHECK(ts.comeOutRolls.repeats() == 0);
    CHECK(ts.comeOutRolls.armed     == false);
    CHECK(ts.pointRolls.count()     == 1);
    CHECK(ts.pointRolls.repeats()   == 0);
    CHECK(ts.pointRolls.armed       == true);

    CHECK(ts.passWinsComeOut.count()        == 0);
    CHECK(ts.passWinsComeOut.repeats()      == 0);
    CHECK(ts.passWinsComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].wins.count()    == 0);
    CHECK(ts.passPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[4].wins.armed      == false);
    CHECK(ts.passPntCnts[5].wins.count()    == 0);
    CHECK(ts.passPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[5].wins.armed      == false);
    CHECK(ts.passPntCnts[6].wins.count()    == 0);
    CHECK(ts.passPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[6].wins.armed      == false);
    CHECK(ts.passPntCnts[8].wins.count()    == 0);
    CHECK(ts.passPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[8].wins.armed      == false);
    CHECK(ts.passPntCnts[9].wins.count()    == 0);
    CHECK(ts.passPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[9].wins.armed      == false);
    CHECK(ts.passPntCnts[10].wins.count()   == 0);
    CHECK(ts.passPntCnts[10].wins.repeats() == 0);
    CHECK(ts.passPntCnts[10].wins.armed     == false);

    CHECK(ts.passLoseComeOut.count()        == 0);
    CHECK(ts.passLoseComeOut.repeats()      == 0);
    CHECK(ts.passLoseComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].lose.count()    == 1);
    CHECK(ts.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[4].lose.armed      == true);
    CHECK(ts.passPntCnts[5].lose.count()    == 0);
    CHECK(ts.passPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[5].lose.armed      == false);
    CHECK(ts.passPntCnts[6].lose.count()    == 0);
    CHECK(ts.passPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[6].lose.armed      == false);
    CHECK(ts.passPntCnts[8].lose.count()    == 0);
    CHECK(ts.passPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[8].lose.armed      == false);
    CHECK(ts.passPntCnts[9].lose.count()    == 0);
    CHECK(ts.passPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[9].lose.armed      == false);
    CHECK(ts.passPntCnts[10].lose.count()   == 0);
    CHECK(ts.passPntCnts[10].lose.repeats() == 0);
    CHECK(ts.passPntCnts[10].lose.armed     == false);

    CHECK(ts.dontPassWinsComeOut.count()        == 0);
    CHECK(ts.dontPassWinsComeOut.repeats()      == 0);
    CHECK(ts.dontPassWinsComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].wins.count()    == 1);
    CHECK(ts.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].wins.armed      == true);
    CHECK(ts.dontPassPntCnts[5].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[6].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[8].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[9].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[10].wins.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].wins.armed     == false);

    CHECK(ts.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.dontPassLoseComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[5].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[6].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[8].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[9].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[10].lose.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].lose.armed     == false);

    CHECK(ts.comeWinsComeOut.count()        == 1);
    CHECK(ts.comeWinsComeOut.repeats()      == 0);
    CHECK(ts.comeWinsComeOut.armed          == true);
    CHECK(ts.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[4].wins.armed      == false);
    CHECK(ts.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[5].wins.armed      == false);
    CHECK(ts.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[6].wins.armed      == false);
    CHECK(ts.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[8].wins.armed      == false);
    CHECK(ts.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[9].wins.armed      == false);
    CHECK(ts.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.comePntCnts[10].wins.armed     == false);

    CHECK(ts.comeLoseComeOut.count()        == 0);
    CHECK(ts.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.comeLoseComeOut.armed          == false);
    CHECK(ts.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[4].lose.armed      == false);
    CHECK(ts.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[5].lose.armed      == false);
    CHECK(ts.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[6].lose.armed      == false);
    CHECK(ts.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[8].lose.armed      == false);
    CHECK(ts.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[9].lose.armed      == false);
    CHECK(ts.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.comePntCnts[10].lose.armed     == false);

    CHECK(ts.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.dontComeWinsComeOut.armed          == false);
    CHECK(ts.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.dontComeLoseComeOut.count()        == 1);
    CHECK(ts.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.dontComeLoseComeOut.armed          == true);
    CHECK(ts.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].lose.armed      == false);
    CHECK(ts.dontComePntCnts[10].lose.count()   == 0);
    CHECK(ts.dontComePntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].lose.armed     == false);

    CHECK(ts.hardwayCounts[4].wins.count()    == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[4].wins.armed      == false);
    CHECK(ts.hardwayCounts[6].wins.count()    == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[6].wins.armed      == false);
    CHECK(ts.hardwayCounts[8].wins.count()    == 0);
    CHECK(ts.hardwayCounts[8].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[8].wins.armed      == false);
    CHECK(ts.hardwayCounts[10].wins.count()   == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.armed     == false);

    CHECK(ts.hardwayCounts[4].lose.count()    == 1);
    CHECK(ts.hardwayCounts[4].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[4].lose.armed      == true);
    CHECK(ts.hardwayCounts[6].lose.count()    == 1);
    CHECK(ts.hardwayCounts[6].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[6].lose.armed      == true);
    CHECK(ts.hardwayCounts[8].lose.count()    == 1);
    CHECK(ts.hardwayCounts[8].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[8].lose.armed      == true);
    CHECK(ts.hardwayCounts[10].lose.count()   == 1);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.armed     == true);

    CHECK(ts.fieldBetWins.count()   == 1);
    CHECK(ts.fieldBetWins.repeats() == 0);
    CHECK(ts.fieldBetWins.armed     == false);
    CHECK(ts.fieldBetLose.count()   == 1);
    CHECK(ts.fieldBetLose.repeats() == 0);
    CHECK(ts.fieldBetLose.armed     == true);

    CHECK(ts.sevenOuts.count()              == 1);
    CHECK(ts.sevenOuts.repeats()            == 0);
    CHECK(ts.sevenOuts.armed                == true);
    CHECK(ts.shooterCounts.count()          == 0);
    CHECK(ts.shooterCounts.repeats()        == 1);
    CHECK(ts.shooterCounts.armed            == false);
    CHECK(ts.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.twosOnComeOutRoll.armed        == false);
    CHECK(ts.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.threesOnComeOutRoll.armed      == false);
    CHECK(ts.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.sevensOnComeOutRoll.armed      == false);
    CHECK(ts.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.crapsOnComeOutRoll.armed       == false);

    // Come out roll, dice = 5,
    // seq: 4,7,5
    point = 0; dice.set(3,2);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 3);
    CHECK(ts.numberCounts[2].count()    == 0);
    CHECK(ts.numberCounts[2].repeats()  == 0);
    CHECK(ts.numberCounts[2].armed      == false);
    CHECK(ts.numberCounts[3].count()    == 0);
    CHECK(ts.numberCounts[3].repeats()  == 0);
    CHECK(ts.numberCounts[3].armed      == false);
    CHECK(ts.numberCounts[4].count()    == 1);
    CHECK(ts.numberCounts[4].repeats()  == 0);
    CHECK(ts.numberCounts[4].armed      == false);
    CHECK(ts.numberCounts[5].count()    == 1);
    CHECK(ts.numberCounts[5].repeats()  == 0);
    CHECK(ts.numberCounts[5].armed      == true);
    CHECK(ts.numberCounts[6].count()    == 0);
    CHECK(ts.numberCounts[6].repeats()  == 0);
    CHECK(ts.numberCounts[6].armed      == false);
    CHECK(ts.numberCounts[7].count()    == 1);
    CHECK(ts.numberCounts[7].repeats()  == 0);
    CHECK(ts.numberCounts[7].armed      == false);
    CHECK(ts.numberCounts[8].count()    == 0);
    CHECK(ts.numberCounts[8].repeats()  == 0);
    CHECK(ts.numberCounts[8].armed      == false);
    CHECK(ts.numberCounts[9].count()    == 0);
    CHECK(ts.numberCounts[9].repeats()  == 0);
    CHECK(ts.numberCounts[9].armed      == false);
    CHECK(ts.numberCounts[10].count()   == 0);
    CHECK(ts.numberCounts[10].repeats() == 0);
    CHECK(ts.numberCounts[10].armed     == false);
    CHECK(ts.numberCounts[11].count()   == 0);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed     == false);
    CHECK(ts.numberCounts[12].count()   == 0);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed     == false);

    CHECK(ts.comeOutRolls.count()   == 2);
    CHECK(ts.comeOutRolls.repeats() == 0);
    CHECK(ts.comeOutRolls.armed     == true);
    CHECK(ts.pointRolls.count()     == 1);
    CHECK(ts.pointRolls.repeats()   == 0);
    CHECK(ts.pointRolls.armed       == false);

    CHECK(ts.passWinsComeOut.count()        == 0);
    CHECK(ts.passWinsComeOut.repeats()      == 0);
    CHECK(ts.passWinsComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].wins.count()    == 0);
    CHECK(ts.passPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[4].wins.armed      == false);
    CHECK(ts.passPntCnts[5].wins.count()    == 0);
    CHECK(ts.passPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[5].wins.armed      == false);
    CHECK(ts.passPntCnts[6].wins.count()    == 0);
    CHECK(ts.passPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[6].wins.armed      == false);
    CHECK(ts.passPntCnts[8].wins.count()    == 0);
    CHECK(ts.passPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[8].wins.armed      == false);
    CHECK(ts.passPntCnts[9].wins.count()    == 0);
    CHECK(ts.passPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[9].wins.armed      == false);
    CHECK(ts.passPntCnts[10].wins.count()   == 0);
    CHECK(ts.passPntCnts[10].wins.repeats() == 0);
    CHECK(ts.passPntCnts[10].wins.armed     == false);

    CHECK(ts.passLoseComeOut.count()        == 0);
    CHECK(ts.passLoseComeOut.repeats()      == 0);
    CHECK(ts.passLoseComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].lose.count()    == 1);
    CHECK(ts.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[4].lose.armed      == true);
    CHECK(ts.passPntCnts[5].lose.count()    == 0);
    CHECK(ts.passPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[5].lose.armed      == false);
    CHECK(ts.passPntCnts[6].lose.count()    == 0);
    CHECK(ts.passPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[6].lose.armed      == false);
    CHECK(ts.passPntCnts[8].lose.count()    == 0);
    CHECK(ts.passPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[8].lose.armed      == false);
    CHECK(ts.passPntCnts[9].lose.count()    == 0);
    CHECK(ts.passPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[9].lose.armed      == false);
    CHECK(ts.passPntCnts[10].lose.count()   == 0);
    CHECK(ts.passPntCnts[10].lose.repeats() == 0);
    CHECK(ts.passPntCnts[10].lose.armed     == false);

    CHECK(ts.dontPassWinsComeOut.count()        == 0);
    CHECK(ts.dontPassWinsComeOut.repeats()      == 0);
    CHECK(ts.dontPassWinsComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].wins.count()    == 1);
    CHECK(ts.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].wins.armed      == true);
    CHECK(ts.dontPassPntCnts[5].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[6].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[8].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[9].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[10].wins.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].wins.armed     == false);

    CHECK(ts.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.dontPassLoseComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[5].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[6].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[8].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[9].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[10].lose.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].lose.armed     == false);

    CHECK(ts.comeWinsComeOut.count()        == 1);
    CHECK(ts.comeWinsComeOut.repeats()      == 0);
    CHECK(ts.comeWinsComeOut.armed          == true);
    CHECK(ts.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[4].wins.armed      == false);
    CHECK(ts.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[5].wins.armed      == false);
    CHECK(ts.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[6].wins.armed      == false);
    CHECK(ts.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[8].wins.armed      == false);
    CHECK(ts.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[9].wins.armed      == false);
    CHECK(ts.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.comePntCnts[10].wins.armed     == false);

    CHECK(ts.comeLoseComeOut.count()        == 0);
    CHECK(ts.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.comeLoseComeOut.armed          == false);
    CHECK(ts.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[4].lose.armed      == false);
    CHECK(ts.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[5].lose.armed      == false);
    CHECK(ts.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[6].lose.armed      == false);
    CHECK(ts.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[8].lose.armed      == false);
    CHECK(ts.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[9].lose.armed      == false);
    CHECK(ts.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.comePntCnts[10].lose.armed     == false);

    CHECK(ts.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.dontComeWinsComeOut.armed          == false);
    CHECK(ts.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.dontComeLoseComeOut.count()        == 1);
    CHECK(ts.dontComeLoseComeOut.repeats()      == 0);
    CHECK(ts.dontComeLoseComeOut.armed          == true);
    CHECK(ts.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].lose.armed      == false);
    CHECK(ts.dontComePntCnts[10].lose.count()   == 0);
    CHECK(ts.dontComePntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].lose.armed     == false);

    CHECK(ts.hardwayCounts[4].wins.count()    == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[4].wins.armed      == false);
    CHECK(ts.hardwayCounts[6].wins.count()    == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[6].wins.armed      == false);
    CHECK(ts.hardwayCounts[8].wins.count()    == 0);
    CHECK(ts.hardwayCounts[8].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[8].wins.armed      == false);
    CHECK(ts.hardwayCounts[10].wins.count()   == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.armed     == false);

    CHECK(ts.hardwayCounts[4].lose.count()    == 1);
    CHECK(ts.hardwayCounts[4].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[4].lose.armed      == true);
    CHECK(ts.hardwayCounts[6].lose.count()    == 1);
    CHECK(ts.hardwayCounts[6].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[6].lose.armed      == true);
    CHECK(ts.hardwayCounts[8].lose.count()    == 1);
    CHECK(ts.hardwayCounts[8].lose.repeats()  == 0);
    CHECK(ts.hardwayCounts[8].lose.armed      == true);
    CHECK(ts.hardwayCounts[10].lose.count()   == 1);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 0);
    CHECK(ts.hardwayCounts[10].lose.armed     == true);

    CHECK(ts.fieldBetWins.count()   == 1);
    CHECK(ts.fieldBetWins.repeats() == 0);
    CHECK(ts.fieldBetWins.armed     == false);
    CHECK(ts.fieldBetLose.count()   == 2);
    CHECK(ts.fieldBetLose.repeats() == 1);
    CHECK(ts.fieldBetLose.armed     == true);

    CHECK(ts.sevenOuts.count()              == 1);
    CHECK(ts.sevenOuts.repeats()            == 0);
    CHECK(ts.sevenOuts.armed                == true);
    CHECK(ts.shooterCounts.count()          == 1);
    CHECK(ts.shooterCounts.repeats()        == 1);
    CHECK(ts.shooterCounts.armed            == true);
    CHECK(ts.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.twosOnComeOutRoll.armed        == false);
    CHECK(ts.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.threesOnComeOutRoll.armed      == false);
    CHECK(ts.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.sevensOnComeOutRoll.armed      == false);
    CHECK(ts.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.crapsOnComeOutRoll.armed       == false);

    // Point roll, seven out dice = 7,
    // seq: 4,7,5,7
    point = 5; dice.set(4,3);
    ts.recordDiceRoll(point, dice);
    CHECK(ts.numRolls == 4);
    CHECK(ts.numberCounts[2].count()    == 0);
    CHECK(ts.numberCounts[2].repeats()  == 0);
    CHECK(ts.numberCounts[2].armed      == false);
    CHECK(ts.numberCounts[3].count()    == 0);
    CHECK(ts.numberCounts[3].repeats()  == 0);
    CHECK(ts.numberCounts[3].armed      == false);
    CHECK(ts.numberCounts[4].count()    == 1);
    CHECK(ts.numberCounts[4].repeats()  == 0);
    CHECK(ts.numberCounts[4].armed      == false);
    CHECK(ts.numberCounts[5].count()    == 1);
    CHECK(ts.numberCounts[5].repeats()  == 0);
    CHECK(ts.numberCounts[5].armed      == false);
    CHECK(ts.numberCounts[6].count()    == 0);
    CHECK(ts.numberCounts[6].repeats()  == 0);
    CHECK(ts.numberCounts[6].armed      == false);
    CHECK(ts.numberCounts[7].count()    == 2);
    CHECK(ts.numberCounts[7].repeats()  == 0);
    CHECK(ts.numberCounts[7].armed      == true);
    CHECK(ts.numberCounts[8].count()    == 0);
    CHECK(ts.numberCounts[8].repeats()  == 0);
    CHECK(ts.numberCounts[8].armed      == false);
    CHECK(ts.numberCounts[9].count()    == 0);
    CHECK(ts.numberCounts[9].repeats()  == 0);
    CHECK(ts.numberCounts[9].armed      == false);
    CHECK(ts.numberCounts[10].count()   == 0);
    CHECK(ts.numberCounts[10].repeats() == 0);
    CHECK(ts.numberCounts[10].armed     == false);
    CHECK(ts.numberCounts[11].count()   == 0);
    CHECK(ts.numberCounts[11].repeats() == 0);
    CHECK(ts.numberCounts[11].armed     == false);
    CHECK(ts.numberCounts[12].count()   == 0);
    CHECK(ts.numberCounts[12].repeats() == 0);
    CHECK(ts.numberCounts[12].armed     == false);

    CHECK(ts.comeOutRolls.count()   == 2);
    CHECK(ts.comeOutRolls.repeats() == 0);
    CHECK(ts.comeOutRolls.armed     == false);
    CHECK(ts.pointRolls.count()     == 2);
    CHECK(ts.pointRolls.repeats()   == 0);
    CHECK(ts.pointRolls.armed       == true);

    CHECK(ts.passWinsComeOut.count()        == 0);
    CHECK(ts.passWinsComeOut.repeats()      == 0);
    CHECK(ts.passWinsComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].wins.count()    == 0);
    CHECK(ts.passPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[4].wins.armed      == false);
    CHECK(ts.passPntCnts[5].wins.count()    == 0);
    CHECK(ts.passPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[5].wins.armed      == false);
    CHECK(ts.passPntCnts[6].wins.count()    == 0);
    CHECK(ts.passPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[6].wins.armed      == false);
    CHECK(ts.passPntCnts[8].wins.count()    == 0);
    CHECK(ts.passPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[8].wins.armed      == false);
    CHECK(ts.passPntCnts[9].wins.count()    == 0);
    CHECK(ts.passPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.passPntCnts[9].wins.armed      == false);
    CHECK(ts.passPntCnts[10].wins.count()   == 0);
    CHECK(ts.passPntCnts[10].wins.repeats() == 0);
    CHECK(ts.passPntCnts[10].wins.armed     == false);

    CHECK(ts.passLoseComeOut.count()        == 0);
    CHECK(ts.passLoseComeOut.repeats()      == 0);
    CHECK(ts.passLoseComeOut.armed          == false);
    CHECK(ts.passPntCnts[4].lose.count()    == 1);
    CHECK(ts.passPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[4].lose.armed      == true);
    CHECK(ts.passPntCnts[5].lose.count()    == 1);
    CHECK(ts.passPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[5].lose.armed      == true);
    CHECK(ts.passPntCnts[6].lose.count()    == 0);
    CHECK(ts.passPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[6].lose.armed      == false);
    CHECK(ts.passPntCnts[8].lose.count()    == 0);
    CHECK(ts.passPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[8].lose.armed      == false);
    CHECK(ts.passPntCnts[9].lose.count()    == 0);
    CHECK(ts.passPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.passPntCnts[9].lose.armed      == false);
    CHECK(ts.passPntCnts[10].lose.count()   == 0);
    CHECK(ts.passPntCnts[10].lose.repeats() == 0);
    CHECK(ts.passPntCnts[10].lose.armed     == false);

    CHECK(ts.dontPassWinsComeOut.count()        == 0);
    CHECK(ts.dontPassWinsComeOut.repeats()      == 0);
    CHECK(ts.dontPassWinsComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].wins.count()    == 1);
    CHECK(ts.dontPassPntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].wins.armed      == true);
    CHECK(ts.dontPassPntCnts[5].wins.count()    == 1);
    CHECK(ts.dontPassPntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].wins.armed      == true);
    CHECK(ts.dontPassPntCnts[6].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[8].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[9].wins.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].wins.armed      == false);
    CHECK(ts.dontPassPntCnts[10].wins.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].wins.armed     == false);

    CHECK(ts.dontPassLoseComeOut.count()        == 0);
    CHECK(ts.dontPassLoseComeOut.repeats()      == 0);
    CHECK(ts.dontPassLoseComeOut.armed          == false);
    CHECK(ts.dontPassPntCnts[4].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[4].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[5].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[5].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[6].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[6].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[8].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[8].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[9].lose.count()    == 0);
    CHECK(ts.dontPassPntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontPassPntCnts[9].lose.armed      == false);
    CHECK(ts.dontPassPntCnts[10].lose.count()   == 0);
    CHECK(ts.dontPassPntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontPassPntCnts[10].lose.armed     == false);

    CHECK(ts.comeWinsComeOut.count()        == 2);
    CHECK(ts.comeWinsComeOut.repeats()      == 1);
    CHECK(ts.comeWinsComeOut.armed          == true);
    CHECK(ts.comePntCnts[4].wins.count()    == 0);
    CHECK(ts.comePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[4].wins.armed      == false);
    CHECK(ts.comePntCnts[5].wins.count()    == 0);
    CHECK(ts.comePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[5].wins.armed      == false);
    CHECK(ts.comePntCnts[6].wins.count()    == 0);
    CHECK(ts.comePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[6].wins.armed      == false);
    CHECK(ts.comePntCnts[8].wins.count()    == 0);
    CHECK(ts.comePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[8].wins.armed      == false);
    CHECK(ts.comePntCnts[9].wins.count()    == 0);
    CHECK(ts.comePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.comePntCnts[9].wins.armed      == false);
    CHECK(ts.comePntCnts[10].wins.count()   == 0);
    CHECK(ts.comePntCnts[10].wins.repeats() == 0);
    CHECK(ts.comePntCnts[10].wins.armed     == false);

    CHECK(ts.comeLoseComeOut.count()        == 0);
    CHECK(ts.comeLoseComeOut.repeats()      == 0);
    CHECK(ts.comeLoseComeOut.armed          == false);
    CHECK(ts.comePntCnts[4].lose.count()    == 0);
    CHECK(ts.comePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[4].lose.armed      == false);
    CHECK(ts.comePntCnts[5].lose.count()    == 0);
    CHECK(ts.comePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[5].lose.armed      == false);
    CHECK(ts.comePntCnts[6].lose.count()    == 0);
    CHECK(ts.comePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[6].lose.armed      == false);
    CHECK(ts.comePntCnts[8].lose.count()    == 0);
    CHECK(ts.comePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[8].lose.armed      == false);
    CHECK(ts.comePntCnts[9].lose.count()    == 0);
    CHECK(ts.comePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.comePntCnts[9].lose.armed      == false);
    CHECK(ts.comePntCnts[10].lose.count()   == 0);
    CHECK(ts.comePntCnts[10].lose.repeats() == 0);
    CHECK(ts.comePntCnts[10].lose.armed     == false);

    CHECK(ts.dontComeWinsComeOut.count()        == 0);
    CHECK(ts.dontComeWinsComeOut.repeats()      == 0);
    CHECK(ts.dontComeWinsComeOut.armed          == false);
    CHECK(ts.dontComePntCnts[4].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[4].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].wins.armed      == false);
    CHECK(ts.dontComePntCnts[5].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[5].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].wins.armed      == false);
    CHECK(ts.dontComePntCnts[6].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[6].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].wins.armed      == false);
    CHECK(ts.dontComePntCnts[8].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[8].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].wins.armed      == false);
    CHECK(ts.dontComePntCnts[9].wins.count()    == 0);
    CHECK(ts.dontComePntCnts[9].wins.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].wins.armed      == false);
    CHECK(ts.dontComePntCnts[10].wins.count()   == 0);
    CHECK(ts.dontComePntCnts[10].wins.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].wins.armed     == false);

    CHECK(ts.dontComeLoseComeOut.count()        == 2);
    CHECK(ts.dontComeLoseComeOut.repeats()      == 1);
    CHECK(ts.dontComeLoseComeOut.armed          == true);
    CHECK(ts.dontComePntCnts[4].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[4].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[4].lose.armed      == false);
    CHECK(ts.dontComePntCnts[5].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[5].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[5].lose.armed      == false);
    CHECK(ts.dontComePntCnts[6].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[6].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[6].lose.armed      == false);
    CHECK(ts.dontComePntCnts[8].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[8].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[8].lose.armed      == false);
    CHECK(ts.dontComePntCnts[9].lose.count()    == 0);
    CHECK(ts.dontComePntCnts[9].lose.repeats()  == 0);
    CHECK(ts.dontComePntCnts[9].lose.armed      == false);
    CHECK(ts.dontComePntCnts[10].lose.count()   == 0);
    CHECK(ts.dontComePntCnts[10].lose.repeats() == 0);
    CHECK(ts.dontComePntCnts[10].lose.armed     == false);

    CHECK(ts.hardwayCounts[4].wins.count()    == 0);
    CHECK(ts.hardwayCounts[4].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[4].wins.armed      == false);
    CHECK(ts.hardwayCounts[6].wins.count()    == 0);
    CHECK(ts.hardwayCounts[6].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[6].wins.armed      == false);
    CHECK(ts.hardwayCounts[8].wins.count()    == 0);
    CHECK(ts.hardwayCounts[8].wins.repeats()  == 0);
    CHECK(ts.hardwayCounts[8].wins.armed      == false);
    CHECK(ts.hardwayCounts[10].wins.count()   == 0);
    CHECK(ts.hardwayCounts[10].wins.repeats() == 0);
    CHECK(ts.hardwayCounts[10].wins.armed     == false);

    CHECK(ts.hardwayCounts[4].lose.count()    == 2);
    CHECK(ts.hardwayCounts[4].lose.repeats()  == 1);
    CHECK(ts.hardwayCounts[4].lose.armed      == true);
    CHECK(ts.hardwayCounts[6].lose.count()    == 2);
    CHECK(ts.hardwayCounts[6].lose.repeats()  == 1);
    CHECK(ts.hardwayCounts[6].lose.armed      == true);
    CHECK(ts.hardwayCounts[8].lose.count()    == 2);
    CHECK(ts.hardwayCounts[8].lose.repeats()  == 1);
    CHECK(ts.hardwayCounts[8].lose.armed      == true);
    CHECK(ts.hardwayCounts[10].lose.count()   == 2);
    CHECK(ts.hardwayCounts[10].lose.repeats() == 1);
    CHECK(ts.hardwayCounts[10].lose.armed     == true);

    CHECK(ts.fieldBetWins.count()   == 1);
    CHECK(ts.fieldBetWins.repeats() == 0);
    CHECK(ts.fieldBetWins.armed     == false);
    CHECK(ts.fieldBetLose.count()   == 3);
    CHECK(ts.fieldBetLose.repeats() == 2);
    CHECK(ts.fieldBetLose.armed     == true);

    CHECK(ts.sevenOuts.count()              == 2);
    CHECK(ts.sevenOuts.repeats()            == 1);
    CHECK(ts.sevenOuts.armed                == true);
    CHECK(ts.shooterCounts.count()          == 0);
    CHECK(ts.shooterCounts.repeats()        == 1);
    CHECK(ts.shooterCounts.armed            == false);
    CHECK(ts.twosOnComeOutRoll.count()      == 0);
    CHECK(ts.twosOnComeOutRoll.repeats()    == 0);
    CHECK(ts.twosOnComeOutRoll.armed        == false);
    CHECK(ts.threesOnComeOutRoll.count()    == 0);
    CHECK(ts.threesOnComeOutRoll.repeats()  == 0);
    CHECK(ts.threesOnComeOutRoll.armed      == false);
    CHECK(ts.sevensOnComeOutRoll.count()    == 0);
    CHECK(ts.sevensOnComeOutRoll.repeats()  == 0);
    CHECK(ts.sevensOnComeOutRoll.armed      == false);
    CHECK(ts.elevensOnComeOutRoll.count()   == 0);
    CHECK(ts.elevensOnComeOutRoll.repeats() == 0);
    CHECK(ts.elevensOnComeOutRoll.armed     == false);
    CHECK(ts.twelvesOnComeOutRoll.count()   == 0);
    CHECK(ts.twelvesOnComeOutRoll.repeats() == 0);
    CHECK(ts.twelvesOnComeOutRoll.armed     == false);
    CHECK(ts.crapsOnComeOutRoll.count()     == 0);
    CHECK(ts.crapsOnComeOutRoll.repeats()   == 0);
    CHECK(ts.crapsOnComeOutRoll.armed       == false);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:five")
{
    // seq 4,5,5,6,6,8,8,9,9,10,10,4,7
    
    TableStats ts("tableId-LasVegas");
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

    CHECK(ts.numPointsEstablished == 1);
    CHECK(ts.comeOutRolls.count() == 2);
    CHECK(ts.pointRolls.count() == 11);
    CHECK(ts.passWinsComeOut.count() == 1);
    CHECK(ts.passLoseComeOut.count() == 0);
    CHECK(ts.dontPassWinsComeOut.count() == 0);
    CHECK(ts.dontPassLoseComeOut.count() == 1);
    CHECK(ts.passPntCnts[4].wins.count() == 1);
    CHECK(ts.dontPassPntCnts[4].lose.count() == 1);
    
    CHECK(ts.comeWinsComeOut.count() == 0);
    CHECK(ts.comeLoseComeOut.count() == 0);
    CHECK(ts.dontComeWinsComeOut.count() == 0);
    CHECK(ts.dontComeLoseComeOut.count() == 0);

    CHECK(ts.comePntCnts[5].wins.count() == 1);
    CHECK(ts.comePntCnts[6].wins.count() == 1);
    CHECK(ts.comePntCnts[8].wins.count() == 1);
    CHECK(ts.comePntCnts[9].wins.count() == 1);
    CHECK(ts.comePntCnts[10].wins.count() == 1);
    
    CHECK(ts.comePntCnts[5].lose.count() == 1);
    CHECK(ts.comePntCnts[6].lose.count() == 1);
    CHECK(ts.comePntCnts[8].lose.count() == 1);
    CHECK(ts.comePntCnts[9].lose.count() == 1);
    CHECK(ts.comePntCnts[10].lose.count() == 1);
    
    CHECK(ts.dontComePntCnts[5].lose.count() == 1);
    CHECK(ts.dontComePntCnts[6].lose.count() == 1);
    CHECK(ts.dontComePntCnts[8].lose.count() == 1);
    CHECK(ts.dontComePntCnts[9].lose.count() == 1);
    CHECK(ts.dontComePntCnts[10].lose.count() == 1);

    CHECK(ts.dontComePntCnts[5].wins.count() == 1);
    CHECK(ts.dontComePntCnts[6].wins.count() == 1);
    CHECK(ts.dontComePntCnts[8].wins.count() == 1);
    CHECK(ts.dontComePntCnts[9].wins.count() == 1);
    CHECK(ts.dontComePntCnts[10].wins.count() == 1);

    CHECK(ts.sevenOuts.count() == 0);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:six")
{
    // seq 4,4,4,4
    
    TableStats ts("tableId-LasVegas");
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

    CHECK(ts.numPointsEstablished == 2);
    CHECK(ts.comeOutRolls.count() == 2);
    CHECK(ts.pointRolls.count() == 2);
    CHECK(ts.passWinsComeOut.count() == 0);
    CHECK(ts.passLoseComeOut.count() == 0);
    CHECK(ts.dontPassWinsComeOut.count() == 0);
    CHECK(ts.dontPassLoseComeOut.count() == 0);
    CHECK(ts.passPntCnts[4].wins.count() == 2);
    CHECK(ts.passPntCnts[4].wins.repeats() == 1);
    CHECK(ts.dontPassPntCnts[4].lose.count() == 2);
    CHECK(ts.dontPassPntCnts[4].lose.repeats() == 1);
    
    CHECK(ts.comeWinsComeOut.count() == 0);
    CHECK(ts.comeLoseComeOut.count() == 0);
    CHECK(ts.dontComeWinsComeOut.count() == 0);
    CHECK(ts.dontComeLoseComeOut.count() == 0);
}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:seven")
{
    // seq 4,5,5,5,5
    
    TableStats ts("tableId-LasVegas");
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

    CHECK(ts.numPointsEstablished == 1);
    CHECK(ts.comeOutRolls.count() == 1);
    CHECK(ts.pointRolls.count() == 4);
    CHECK(ts.passWinsComeOut.count() == 0);
    CHECK(ts.passLoseComeOut.count() == 0);
    CHECK(ts.dontPassWinsComeOut.count() == 0);
    CHECK(ts.dontPassLoseComeOut.count() == 0);
    CHECK(ts.passPntCnts[4].wins.count() == 0);
    CHECK(ts.passPntCnts[4].wins.repeats() == 0);
    CHECK(ts.dontPassPntCnts[4].lose.count() == 0);
    CHECK(ts.dontPassPntCnts[4].lose.repeats() == 0);
    
    CHECK(ts.comeWinsComeOut.count() == 0);
    CHECK(ts.comeLoseComeOut.count() == 0);
    CHECK(ts.dontComeWinsComeOut.count() == 0);
    CHECK(ts.dontComeLoseComeOut.count() == 0);

    CHECK(ts.comePntCnts[5].wins.count() == 3);
    CHECK(ts.comePntCnts[5].wins.repeats() == 2);
    CHECK(ts.dontComePntCnts[5].lose.count() == 3);
    CHECK(ts.dontComePntCnts[5].lose.repeats() == 2);

}

//----------------------------------------------------------------

TEST_CASE("TableStats:diceroll:eight")
{
    // seq 4,4,4,4,4,4
    
    TableStats ts("tableId-LasVegas");
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

    CHECK(ts.numPointsEstablished == 3);
    CHECK(ts.comeOutRolls.count() == 3);
    CHECK(ts.pointRolls.count() == 3);
    CHECK(ts.passWinsComeOut.count() == 0);
    CHECK(ts.passLoseComeOut.count() == 0);
    CHECK(ts.dontPassWinsComeOut.count() == 0);
    CHECK(ts.dontPassLoseComeOut.count() == 0);
    CHECK(ts.passPntCnts[4].wins.count() == 3);
    CHECK(ts.passPntCnts[4].wins.repeats() == 2);
    CHECK(ts.dontPassPntCnts[4].lose.count() == 3);
    CHECK(ts.dontPassPntCnts[4].lose.repeats() == 2);
    
    CHECK(ts.comeWinsComeOut.count() == 0);
    CHECK(ts.comeLoseComeOut.count() == 0);
    CHECK(ts.dontComeWinsComeOut.count() == 0);
    CHECK(ts.dontComeLoseComeOut.count() == 0);

    CHECK(ts.comePntCnts[4].wins.count() == 4);
    CHECK(ts.comePntCnts[4].wins.repeats() == 3);
    CHECK(ts.dontComePntCnts[4].lose.count() == 4);
    CHECK(ts.dontComePntCnts[4].lose.repeats() == 3);
}

//----------------------------------------------------------------
