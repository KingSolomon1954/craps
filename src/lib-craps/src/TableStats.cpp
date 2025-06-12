//----------------------------------------------------------------
//
// File: TableStats.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <algorithm>

using namespace Craps;

/*-----------------------------------------------------------*//**

Update lots of stats after dice throw.

*/
void
TableStats::updateDiceRoll(unsigned point,
                           const Dice& curRoll,
                           const Dice& prevRoll)
{
    prevRoll_ = prevRoll;  // Cache local copy in member var.

    if (point == 0)
    {
        incrementComeOutRolls();
    }
    else
    {
        incrementNonComeOutRolls();
    }

    unsigned roll = curRoll.value();
    unsigned d1   = curRoll.d1();
    unsigned d2   = curRoll.d2();
    
    if (roll == point)
    {
        incrementPassLineWins();
        incrementDontPassLoss();
    }

    switch(roll)
    {
        case 2:  update2 (point);  break;
        case 3:  update3 (point);  break;
        case 4:  update4 (d1, d2); break;
        case 5:  update5 ();       break;
        case 6:  update6 (d1, d2); break;
        case 7:  update7 (point);  break;
        case 8:  update8 (d1, d2); break;
        case 9:  update9 ();       break;
        case 10: update10(d1, d2); break;
        case 11: update11(point);  break;
        case 12: update12(point);  break;
    }
}

//----------------------------------------------------------------

void
TableStats::update2(unsigned point)
{
    num2++;
    if (prevRoll_.value() == 2)
    {
        numCurCnsectvNum2++;
        numMaxCnsectvNum2 = std::max(numMaxCnsectvNum2, numCurCnsectvNum2);
    }
    else
    {
        numCurCnsectvNum2 = 0;
    }
    incrementRollsThisShooter();
    if (point == 0)
    {
        incrementPassLineLoss();
        incrementDontPassWins();
        incrementTwosOnComeOutRoll();
        incrementCrapsOnComeOutRoll();
    }
    incrementFieldBetWins();
}

//-----------------------------------------------------------------

void
TableStats::update3(unsigned point)
{
    num3++;
    if (prevRoll_.value() == 3)
    {
        numCurCnsectvNum3++;
        numMaxCnsectvNum3 = std::max(numMaxCnsectvNum3, numCurCnsectvNum3);
    }
    else
    {
        numCurCnsectvNum3 = 0;
    }
    incrementRollsThisShooter();
    if (point == 0)
    {
        incrementPassLineLoss();
        incrementDontPassWins();
        incrementThreesOnComeOutRoll();
        incrementCrapsOnComeOutRoll();
    }
    incrementFieldBetWins();
}

//-----------------------------------------------------------------

void
TableStats::update4(unsigned d1, unsigned d2)
{
    num4++;
    if (prevRoll_.value() == 4)
    {
        numCurCnsectvNum4++;
        numMaxCnsectvNum4 = std::max(numMaxCnsectvNum4, numCurCnsectvNum4);
    }
    else
    {
        numCurCnsectvNum4 = 0;
    }
    incrementRollsThisShooter();
    if (armed4)
    {
        incrementComeWinsOn4();
        incrementDontLossOn4();
    }
    armed4 = true;
    incrementFieldBetWins();
    if (d1 == d2)
    {
        incrementHardwayWinsOn4();
    }
    else
    {
        incrementHardwayLossOn4();
    }
    resetComeOutRollCounters();
}

//-----------------------------------------------------------------

void
TableStats::update5()
{
    num5++;
    if (prevRoll_.value() == 5)
    {
        numCurCnsectvNum5++;
        numMaxCnsectvNum5 = std::max(numMaxCnsectvNum5, numCurCnsectvNum5);
    }
    else
    {
        numCurCnsectvNum5 = 0;
    }
    incrementRollsThisShooter();
    if (armed5)
    {
        incrementComeWinsOn5();
        incrementDontLossOn5();
    }
    armed5 = true;
    incrementFieldBetLoss();
    resetComeOutRollCounters();
}

//-----------------------------------------------------------------

void
TableStats::update6(unsigned d1, unsigned d2)
{
    num6++;
    if (prevRoll_.value() == 6)
    {
        numCurCnsectvNum6++;
        numMaxCnsectvNum6 = std::max(numMaxCnsectvNum6, numCurCnsectvNum6);
    }
    else
    {
        numCurCnsectvNum6 = 0;
    }
    incrementRollsThisShooter();
    if (armed6)
    {
        incrementComeWinsOn6();
        incrementDontLossOn6();
    }
    armed6 = true;
    incrementFieldBetLoss();
    if (d1 == d2)
    {
        incrementHardwayWinsOn6();
    }
    else
    {
        incrementHardwayLossOn6();
    }
    resetComeOutRollCounters();
}
            
//-----------------------------------------------------------------

void
TableStats::update7(unsigned point)
{
    num7++;
    if (prevRoll_.value() == 7)
    {
        numCurCnsectvNum7++;
        numMaxCnsectvNum7 = std::max(numMaxCnsectvNum7, numCurCnsectvNum7);
    }
    else
    {
        numCurCnsectvNum7 = 0;
    }
    
    if (armed4)
    {
        armed4 = false;
        incrementComeLossOn4();
        incrementDontWinsOn4();
    }

    if (armed5)
    {
        armed5 = false;
        incrementComeLossOn5();
        incrementDontWinsOn5();
    }

    if (armed6)
    {
        armed6 = false;
        incrementComeLossOn6();
        incrementDontWinsOn6();
    }

    if (armed8)
    {
        armed8 = false;
        incrementComeLossOn8();
        incrementDontWinsOn8();
    }

    if (armed9)
    {
        armed9 = false;
        incrementComeLossOn9();
        incrementDontWinsOn9();
    }

    if (armed10)
    {
        armed10 = false;
        incrementComeLossOn10();
        incrementDontWinsOn10();
    }

    incrementHardwayLossOn4();
    incrementHardwayLossOn6();
    incrementHardwayLossOn8();
    incrementHardwayLossOn10();

    incrementFieldBetLoss();

    if (point == 0)
    {
        incrementPassLineWins();
        incrementDontPassLoss();
        incrementSevensOnComeOutRoll();
        incrementRollsThisShooter();
    }
    else
    {
        incrementPassLineLoss();
        incrementDontPassWins();
        incrementSevenOut();
    }
}

//-----------------------------------------------------------------

void
TableStats::update8(unsigned d1, unsigned d2)
{
    num8++;
    if (prevRoll_.value() == 8)
    {
        numCurCnsectvNum8++;
        numMaxCnsectvNum8 = std::max(numMaxCnsectvNum8, numCurCnsectvNum8);
    }
    else
    {
        numCurCnsectvNum8 = 0;
    }
    
    incrementRollsThisShooter();
    if (armed8)
    {
        incrementComeWinsOn8();
        incrementDontLossOn8();
    }
    armed8 = true;
    incrementFieldBetLoss();
    if (d1 == d2)
    {
        incrementHardwayWinsOn8();
    }
    else
    {
        incrementHardwayLossOn8();
    }
    resetComeOutRollCounters();
}

//-----------------------------------------------------------------

void
TableStats::update9()
{
    num9++;
    if (prevRoll_.value() == 9)
    {
        numCurCnsectvNum9++;
        numMaxCnsectvNum9 = std::max(numMaxCnsectvNum9, numCurCnsectvNum9);
    }
    else
    {
        numCurCnsectvNum9 = 0;
    }
    
    incrementRollsThisShooter();
    if (armed9)
    {
        incrementComeWinsOn9();
        incrementDontLossOn9();            
    }
    armed9 = true;
    incrementFieldBetWins();
    resetComeOutRollCounters();
}

//-----------------------------------------------------------------

void
TableStats::update10(unsigned d1, unsigned d2)
{
    num10++;
    if (prevRoll_.value() == 10)
    {
        numCurCnsectvNum10++;
        numMaxCnsectvNum10 = std::max(numMaxCnsectvNum10, numCurCnsectvNum10);
    }
    else
    {
        numCurCnsectvNum10 = 0;
    }
    
    incrementRollsThisShooter();
    if (armed10)
    {
        incrementComeWinsOn10();
        incrementDontLossOn10();
    }
    armed10 = true;
    incrementFieldBetWins();
    if (d1 == d2)
    {
        incrementHardwayWinsOn10();
    }
    else
    {
        incrementHardwayLossOn10();
    }
    resetComeOutRollCounters();
}

//-----------------------------------------------------------------

void
TableStats::update11(unsigned point)
{
    num11++;
    if (prevRoll_.value() == 11)
    {
        numCurCnsectvNum11++;
        numMaxCnsectvNum11 = std::max(numMaxCnsectvNum11, numCurCnsectvNum11);
    }
    else
    {
        numCurCnsectvNum11 = 0;
    }
    
    incrementRollsThisShooter();
    if (point == 0)
    {
        incrementPassLineWins();
        incrementDontPassLoss();
        incrementElevensOnComeOutRoll();
    }
    incrementFieldBetWins();
}

//-----------------------------------------------------------------

void
TableStats::update12(unsigned point)
{
    num12++;
    if (prevRoll_.value() == 12)
    {
        numCurCnsectvNum12++;
        numMaxCnsectvNum12 = std::max(numMaxCnsectvNum12, numCurCnsectvNum12);
    }
    else
    {
        numCurCnsectvNum12 = 0;
    }
    
    incrementRollsThisShooter();
    if (point == 0)
    {
        incrementPassLineLoss();
        incrementCrapsOnComeOutRoll();
        incrementTwelvesOnComeOutRoll();
    }
    incrementFieldBetWins();
}

//----------------------------------------------------------------

void
TableStats::incrementComeOutRolls()
{
    numComeOutRolls++;
    numCurCnsectvComeOutRolls++;
    numMaxCnsectvComeOutRolls =
        std::max(numMaxCnsectvComeOutRolls, numCurCnsectvComeOutRolls);
    numCurCnsectvNonComeOutRolls = 0;
}

//----------------------------------------------------------------

void
TableStats::incrementNonComeOutRolls()
{
    numNonComeOutRolls++;
    numCurCnsectvNonComeOutRolls++;
    numMaxCnsectvNonComeOutRolls =
        std::max(numMaxCnsectvNonComeOutRolls,
                 numCurCnsectvNonComeOutRolls);
    numCurCnsectvComeOutRolls = 0;
}

//----------------------------------------------------------------

void
TableStats::incrementPassLineWins()
{
    numPassLineWins++;
    numCurCnsectvPassLineWins++;
    numMaxCnsectvPassLineWins = std::max(numMaxCnsectvPassLineWins,
                                         numCurCnsectvPassLineWins);
    numCurCnsectvPassLineLoss = 0;
}

//----------------------------------------------------------------

void
TableStats::incrementPassLineLoss()
{
    numPassLineLoss++;
    numCurCnsectvPassLineLoss++;
    numMaxCnsectvPassLineLoss = std::max(numMaxCnsectvPassLineLoss,
                                         numCurCnsectvPassLineLoss);
    numCurCnsectvPassLineWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontPassWins()
{
    numDontPassWins++;
    numCurCnsectvDontPassWins++;
    numMaxCnsectvDontPassWins = std::max(numMaxCnsectvDontPassWins,
                                         numCurCnsectvDontPassWins);
    numCurCnsectvDontPassLoss = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontPassLoss()
{
    numDontPassLoss++;
    numCurCnsectvDontPassLoss++;
    numMaxCnsectvDontPassLoss = std::max(numMaxCnsectvDontPassLoss,
                                         numCurCnsectvDontPassLoss);
    numCurCnsectvDontPassWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementFieldBetWins()
{
    numFieldBetWins++;
    numCurCnsectvFieldBetWins++;
    numMaxCnsectvFieldBetWins = std::max(numMaxCnsectvFieldBetWins,
                                         numCurCnsectvFieldBetWins);
    numCurCnsectvFieldBetLoss = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementFieldBetLoss()
{
    numFieldBetLoss++;
    numCurCnsectvFieldBetLoss++;
    numMaxCnsectvFieldBetLoss = std::max(numMaxCnsectvFieldBetLoss,
                                         numCurCnsectvFieldBetLoss);
    numCurCnsectvFieldBetWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayWinsOn4()
{
    numHardwayWinsOn4++;
    numCurCnsectvHardwayWinsOn4++;
    numMaxCnsectvHardwayWinsOn4 = std::max(numMaxCnsectvHardwayWinsOn4,
                                           numCurCnsectvHardwayWinsOn4);
    numCurCnsectvHardwayLossOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayLossOn4()
{
    numHardwayLossOn4++;
    numCurCnsectvHardwayLossOn4++;
    numMaxCnsectvHardwayLossOn4 = std::max(numMaxCnsectvHardwayLossOn4,
                                           numCurCnsectvHardwayLossOn4);
    numCurCnsectvHardwayWinsOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayWinsOn6()
{
    numHardwayWinsOn6++;
    numCurCnsectvHardwayWinsOn6++;
    numMaxCnsectvHardwayWinsOn6 = std::max(numMaxCnsectvHardwayWinsOn6,
                                           numCurCnsectvHardwayWinsOn6);
    numCurCnsectvHardwayLossOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayLossOn6()
{
    numHardwayLossOn6++;
    numCurCnsectvHardwayLossOn6++;
    numMaxCnsectvHardwayLossOn6 = std::max(numMaxCnsectvHardwayLossOn6,
                                           numCurCnsectvHardwayLossOn6);
    numCurCnsectvHardwayWinsOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayWinsOn8()
{
    numHardwayWinsOn8++;
    numCurCnsectvHardwayWinsOn8++;
    numMaxCnsectvHardwayWinsOn8 = std::max(numMaxCnsectvHardwayWinsOn8,
                                           numCurCnsectvHardwayWinsOn8);
    numCurCnsectvHardwayLossOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayLossOn8()
{
    numHardwayLossOn8++;
    numCurCnsectvHardwayLossOn8++;
    numMaxCnsectvHardwayLossOn8 = std::max(numMaxCnsectvHardwayLossOn8,
                                           numCurCnsectvHardwayLossOn8);
    numCurCnsectvHardwayWinsOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayWinsOn10()
{
    numHardwayWinsOn10++;
    numCurCnsectvHardwayWinsOn10++;
    numMaxCnsectvHardwayWinsOn10 = std::max(numMaxCnsectvHardwayWinsOn10,
                                            numCurCnsectvHardwayWinsOn10);
    numCurCnsectvHardwayLossOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementHardwayLossOn10()
{
    numHardwayLossOn10++;
    numCurCnsectvHardwayLossOn10++;
    numMaxCnsectvHardwayLossOn10 = std::max(numMaxCnsectvHardwayLossOn10,
                                            numCurCnsectvHardwayLossOn10);
    numCurCnsectvHardwayWinsOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeWinsOn4()
{
    numComeWinsOn4++;
    numCurCnsectvComeWinsOn4++;
    numMaxCnsectvComeWinsOn4 = std::max(numMaxCnsectvComeWinsOn4,
                                        numCurCnsectvComeWinsOn4);
    numCurCnsectvComeLossOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeWinsOn5()
{
    numComeWinsOn5++;
    numCurCnsectvComeWinsOn5++;
    numMaxCnsectvComeWinsOn5 = std::max(numMaxCnsectvComeWinsOn5,
                                        numCurCnsectvComeWinsOn5);
    numCurCnsectvComeLossOn5 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeWinsOn6()
{
    numComeWinsOn6++;
    numCurCnsectvComeWinsOn6++;
    numMaxCnsectvComeWinsOn6 = std::max(numMaxCnsectvComeWinsOn6,
                                        numCurCnsectvComeWinsOn6);
    numCurCnsectvComeLossOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeWinsOn8()
{
    numComeWinsOn8++;
    numCurCnsectvComeWinsOn8++;
    numMaxCnsectvComeWinsOn8 = std::max(numMaxCnsectvComeWinsOn8,
                                        numCurCnsectvComeWinsOn8);
    numCurCnsectvComeLossOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeWinsOn9()
{
    numComeWinsOn9++;
    numCurCnsectvComeWinsOn9++;
    numMaxCnsectvComeWinsOn9 = std::max(numMaxCnsectvComeWinsOn9,
                                        numCurCnsectvComeWinsOn9);
    numCurCnsectvComeLossOn9 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeWinsOn10()
{
    numComeWinsOn10++;
    numCurCnsectvComeWinsOn10++;
    numMaxCnsectvComeWinsOn10 = std::max(numMaxCnsectvComeWinsOn10,
                                         numCurCnsectvComeWinsOn10);
    numCurCnsectvComeLossOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeLossOn4()
{
    numComeLossOn4++;
    numCurCnsectvComeLossOn4++;
    numMaxCnsectvComeLossOn4 = std::max(numMaxCnsectvComeLossOn4,
                                        numCurCnsectvComeLossOn4);
    numCurCnsectvComeWinsOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeLossOn5()
{
    numComeLossOn5++;
    numCurCnsectvComeLossOn5++;
    numMaxCnsectvComeLossOn5 = std::max(numMaxCnsectvComeLossOn5,
                                        numCurCnsectvComeLossOn5);
    numCurCnsectvComeWinsOn5 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeLossOn6()
{
    numComeLossOn6++;
    numCurCnsectvComeLossOn6++;
    numMaxCnsectvComeLossOn6 = std::max(numMaxCnsectvComeLossOn6,
                                        numCurCnsectvComeLossOn6);
    numCurCnsectvComeWinsOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeLossOn8()
{
    numComeLossOn8++;
    numCurCnsectvComeLossOn8++;
    numMaxCnsectvComeLossOn8 = std::max(numMaxCnsectvComeLossOn8,
                                        numCurCnsectvComeLossOn8);
    numCurCnsectvComeWinsOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeLossOn9()
{
    numComeLossOn9++;
    numCurCnsectvComeLossOn9++;
    numMaxCnsectvComeLossOn9 = std::max(numMaxCnsectvComeLossOn9,
                                        numCurCnsectvComeLossOn9);
    numCurCnsectvComeWinsOn9 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementComeLossOn10()
{
    numComeLossOn10++;
    numCurCnsectvComeLossOn10++;
    numMaxCnsectvComeLossOn10 = std::max(numMaxCnsectvComeLossOn10,
                                         numCurCnsectvComeLossOn10);
    numCurCnsectvComeWinsOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontWinsOn4()
{
    numDontWinsOn4++;
    numCurCnsectvDontWinsOn4++;
    numMaxCnsectvDontWinsOn4 = std::max(numMaxCnsectvDontWinsOn4,
                                        numCurCnsectvDontWinsOn4);
    numCurCnsectvDontLossOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontWinsOn5()
{
    numDontWinsOn5++;
    numCurCnsectvDontWinsOn5++;
    numMaxCnsectvDontWinsOn5 = std::max(numMaxCnsectvDontWinsOn5,
                                        numCurCnsectvDontWinsOn5);
    numCurCnsectvDontLossOn5 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontWinsOn6()
{
    numDontWinsOn6++;
    numCurCnsectvDontWinsOn6++;
    numMaxCnsectvDontWinsOn6 = std::max(numMaxCnsectvDontWinsOn6,
                                        numCurCnsectvDontWinsOn6);
    numCurCnsectvDontLossOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontWinsOn8()
{
    numDontWinsOn8++;
    numCurCnsectvDontWinsOn8++;
    numMaxCnsectvDontWinsOn8 = std::max(numMaxCnsectvDontWinsOn8,
                                        numCurCnsectvDontWinsOn8);
    numCurCnsectvDontLossOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontWinsOn9()
{
    numDontWinsOn9++;
    numCurCnsectvDontWinsOn9++;
    numMaxCnsectvDontWinsOn9 = std::max(numMaxCnsectvDontWinsOn9,
                                        numCurCnsectvDontWinsOn9);
    numCurCnsectvDontLossOn9 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontWinsOn10()
{
    numDontWinsOn10++;
    numCurCnsectvDontWinsOn10++;
    numMaxCnsectvDontWinsOn10 = std::max(numMaxCnsectvDontWinsOn10,
                                         numCurCnsectvDontWinsOn10);
    numCurCnsectvDontLossOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontLossOn4()
{
    numDontLossOn4++;
    numCurCnsectvDontLossOn4++;
    numMaxCnsectvDontLossOn4 = std::max(numMaxCnsectvDontLossOn4,
                                        numCurCnsectvDontLossOn4);
    numCurCnsectvDontWinsOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontLossOn5()
{
    numDontLossOn5++;
    numCurCnsectvDontLossOn5++;
    numMaxCnsectvDontLossOn5 = std::max(numMaxCnsectvDontLossOn5,
                                        numCurCnsectvDontLossOn5);
    numCurCnsectvDontWinsOn5 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontLossOn6()
{
    numDontLossOn6++;
    numCurCnsectvDontLossOn6++;
    numMaxCnsectvDontLossOn6 = std::max(numMaxCnsectvDontLossOn6,
                                        numCurCnsectvDontLossOn6);
    numCurCnsectvDontWinsOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontLossOn8()
{
    numDontLossOn8++;
    numCurCnsectvDontLossOn8++;
    numMaxCnsectvDontLossOn8 = std::max(numMaxCnsectvDontLossOn8,
                                        numCurCnsectvDontLossOn8);
    numCurCnsectvDontWinsOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontLossOn9()
{
    numDontLossOn9++;
    numCurCnsectvDontLossOn9++;
    numMaxCnsectvDontLossOn9 = std::max(numMaxCnsectvDontLossOn9,
                                        numCurCnsectvDontLossOn9);
    numCurCnsectvDontWinsOn9 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementDontLossOn10()
{
    numDontLossOn10++;
    numCurCnsectvDontLossOn10++;
    numMaxCnsectvDontLossOn10 = std::max(numMaxCnsectvDontLossOn10,
                                         numCurCnsectvDontLossOn10);
    numCurCnsectvDontWinsOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::incrementCrapsOnComeOutRoll()
{
    numCrapsOnComeOutRoll++;
    numCurCnsectvCrapsOnComeOutRoll++;
    numMaxCnsectvCrapsOnComeOutRoll = std::max(numMaxCnsectvCrapsOnComeOutRoll,
                                               numCurCnsectvCrapsOnComeOutRoll);
}

//-----------------------------------------------------------------

void
TableStats::incrementTwosOnComeOutRoll()
{
    numTwosOnComeOutRoll++;
    numCurCnsectvTwosOnComeOutRoll++;
    numMaxCnsectvTwosOnComeOutRoll = std::max(numMaxCnsectvTwosOnComeOutRoll,
                                              numCurCnsectvTwosOnComeOutRoll);
}

//-----------------------------------------------------------------

void
TableStats::incrementThreesOnComeOutRoll()
{
    numThreesOnComeOutRoll++;
    numCurCnsectvThreesOnComeOutRoll++;
    numMaxCnsectvThreesOnComeOutRoll =
        std::max(numMaxCnsectvThreesOnComeOutRoll,
                 numCurCnsectvThreesOnComeOutRoll);
}

//-----------------------------------------------------------------

void
TableStats::incrementSevensOnComeOutRoll()
{
    numSevensOnComeOutRoll++;
    numCurCnsectvSevensOnComeOutRoll++;
    numMaxCnsectvSevensOnComeOutRoll =
        std::max(numMaxCnsectvSevensOnComeOutRoll,
                 numCurCnsectvSevensOnComeOutRoll);
}

//-----------------------------------------------------------------

void
TableStats::incrementElevensOnComeOutRoll()
{
    numElevensOnComeOutRoll++;
    numCurCnsectvElevensOnComeOutRoll++;
    numMaxCnsectvElevensOnComeOutRoll =
        std::max(numMaxCnsectvElevensOnComeOutRoll,
                 numCurCnsectvElevensOnComeOutRoll);
}

//-----------------------------------------------------------------

void
TableStats::incrementTwelvesOnComeOutRoll()
{
    numTwelvesOnComeOutRoll++;
    numCurCnsectvTwelvesOnComeOutRoll++;
    numMaxCnsectvTwelvesOnComeOutRoll =
        std::max(numMaxCnsectvTwelvesOnComeOutRoll,
                 numCurCnsectvTwelvesOnComeOutRoll);
}

//-----------------------------------------------------------------

void
TableStats::incrementRollsThisShooter()
{
    numCurCnsectvRollsThisShooter++;
    numMaxCnsectvRollsThisShooter = std::max(numMaxCnsectvRollsThisShooter,
                                             numCurCnsectvRollsThisShooter);
}

//-----------------------------------------------------------------

void
TableStats::incrementSevenOut()
{
    numSevenOut++;
    // avgNumSevenOuts = (double)numSevenOut / (double)numRolls;
    
    numTurnsShooter++;           // Same as sevenOut count
    totCnsectvRollsPerShooter += numCurCnsectvRollsThisShooter;
    numCurCnsectvRollsThisShooter = 0;
    // avgNumRollsPerShooter = (double)totCnsectvRollsPerShooter /
    //                         (double)numTurnsShooter;
}

//-----------------------------------------------------------------

void
TableStats::resetComeOutRollCounters()
{
    numCurCnsectvComeOutRolls         = 0;
    numCurCnsectvSevensOnComeOutRoll  = 0;
    numCurCnsectvElevensOnComeOutRoll = 0;
    numCurCnsectvTwosOnComeOutRoll    = 0;
    numCurCnsectvThreesOnComeOutRoll  = 0;
    numCurCnsectvTwelvesOnComeOutRoll = 0;
    numCurCnsectvCrapsOnComeOutRoll   = 0;
}

/*-----------------------------------------------------------*//**

Update per add bet.

*/
void
TableStats::updateAddBet(Gbl::Money betAmount)
{
    numBetsMade++;
    totAmtAllBets += betAmount;
    maxAmtOneBet = std::max(betAmount, maxAmtOneBet);
    // avgAmtPerBet = (double)totAmtAllBets / (double)numBetsMade;
}

/*-----------------------------------------------------------*//**

Update per odds bet.

*/
void
TableStats::updateOddsBet(Gbl::Money contractAmount, Gbl::Money oddsAmount)
{
    Gbl::Money amount = contractAmount + oddsAmount;
    totAmtAllBets += oddsAmount;  // Avoid double counting contract amount
    maxAmtOneBet = std::max(amount, maxAmtOneBet);
    // avgAmtPerBet = (double)totAmtAllBets / (double)numBetsMade;
}

/*-----------------------------------------------------------*//**

Update various betting stats after dice throw.

*/
void
TableStats::updateBetsAfterThrow(
    Gbl::Money amtOnTable,
    const std::pair<unsigned, Gbl::Money>& winStats,
    const std::pair<unsigned, Gbl::Money>& loseStats,
    unsigned numBetsKeepThisRoll)
{
    Gbl::Money amtWinThisRoll    = winStats.second;
    Gbl::Money amtLoseThisRoll   = loseStats.second;
    unsigned numBetsWinThisRoll  = winStats.first;
    unsigned numBetsLoseThisRoll = loseStats.first;
    unsigned numBetsThisRoll     = numBetsWinThisRoll  +
                                   numBetsLoseThisRoll +
                                   numBetsKeepThisRoll;
    
    numBetsWin  += numBetsWinThisRoll;
    numBetsLose += numBetsLoseThisRoll;
    numBetsKeep += numBetsKeepThisRoll;
    
    maxNumBetsPerRoll = std::max(numBetsThisRoll, maxNumBetsPerRoll);
    totNumBetsPerRoll += numBetsThisRoll;
    // avgNumBetsPerRoll = (double)totNumBetsPerRoll / (double)numRolls;

    maxNumBetsWinPerRoll = std::max(numBetsWinThisRoll, maxNumBetsWinPerRoll);
    totNumBetsWinPerRoll += numBetsWinThisRoll;
    // avgNumBetsWinPerRoll = (double)totNumBetsWinPerRoll / (double)numRolls;

    maxNumBetsLosePerRoll = std::max(numBetsLoseThisRoll, maxNumBetsLosePerRoll);
    totNumBetsLosePerRoll += numBetsLoseThisRoll;
    // avgNumBetsLosePerRoll = (double)totNumBetsLosePerRoll / (double)numRolls;

    maxNumBetsKeepPerRoll = std::max(numBetsKeepThisRoll, maxNumBetsKeepPerRoll);
    totNumBetsKeepPerRoll += numBetsKeepThisRoll;
    // avgNumBetsKeepPerRoll = (double)totNumBetsKeepPerRoll / (double)numRolls;

    maxAmtBetOneRoll = std::max(amtOnTable, maxAmtBetOneRoll);
    totAmtBetPerRoll += amtOnTable;
    // avgAmtBetPerRoll = (double)totAmtBetPerRoll / (double)numRolls;

    maxAmtWinOneRoll = std::max(amtWinThisRoll, maxAmtWinOneRoll);
    totAmtWinPerRoll += amtWinThisRoll;
    // avgAmtWinPerRoll = (double)totAmtWinPerRoll / (double)numRolls;
    
    maxAmtLoseOneRoll = std::max(amtLoseThisRoll, maxAmtLoseOneRoll);
    totAmtLosePerRoll += amtLoseThisRoll;
    // avgAmtLosePerRoll = (double)totAmtLosePerRoll / (double)numRolls;

    totAmtWin  += amtWinThisRoll;
    totAmtLose += amtLoseThisRoll;
    // balance = totAmtWin - totAmtLose;
}

//-----------------------------------------------------------------

void
TableStats::reset()
{
    // Betting Stats
    numBetsMade = 0;
    numBetsWin  = 0;
    numBetsLose = 0;
    numBetsKeep = 0;
    maxNumBetsPerRoll     = 0;
    totNumBetsPerRoll     = 0;
    maxNumBetsWinPerRoll  = 0;
    totNumBetsWinPerRoll  = 0;
    maxNumBetsLosePerRoll = 0;
    totNumBetsLosePerRoll = 0;
    maxNumBetsKeepPerRoll = 0;
    totNumBetsKeepPerRoll = 0;
    maxAmtOneBet          = 0;
    totAmtAllBets         = 0;
    maxAmtBetOneRoll      = 0;
    totAmtBetPerRoll      = 0;
    maxAmtWinOneRoll      = 0;
    totAmtWinPerRoll      = 0;
    maxAmtLoseOneRoll     = 0;
    totAmtLosePerRoll     = 0;
    totAmtWin             = 0;
    totAmtLose            = 0;

    // Dice Roll Stats
    numRolls                      = 0;
    numSevenOut                   = 0;
    
    numComeOutRolls              = 0;
    numNonComeOutRolls           = 0;
    numCurCnsectvComeOutRolls    = 0;
    numCurCnsectvNonComeOutRolls = 0;
    numMaxCnsectvComeOutRolls    = 0;
    numMaxCnsectvNonComeOutRolls = 0;

    numPassLineWins = 0;
    numPassLineLoss = 0;
    numDontPassWins = 0;
    numDontPassLoss = 0;
    numFieldBetWins = 0;
    numFieldBetLoss = 0;

    numCurCnsectvPassLineWins = 0;
    numCurCnsectvPassLineLoss = 0;
    numCurCnsectvDontPassWins = 0;
    numCurCnsectvDontPassLoss = 0;
    numCurCnsectvFieldBetWins = 0;
    numCurCnsectvFieldBetLoss = 0;

    numMaxCnsectvPassLineWins = 0;
    numMaxCnsectvPassLineLoss = 0;
    numMaxCnsectvDontPassWins = 0;
    numMaxCnsectvDontPassLoss = 0;
    numMaxCnsectvFieldBetWins = 0;
    numMaxCnsectvFieldBetLoss = 0;

    numSevensOnComeOutRoll  = 0;
    numElevensOnComeOutRoll = 0;
    numTwosOnComeOutRoll    = 0;
    numThreesOnComeOutRoll  = 0;
    numTwelvesOnComeOutRoll = 0;
    numCrapsOnComeOutRoll   = 0;

    numCurCnsectvSevensOnComeOutRoll  = 0;
    numCurCnsectvElevensOnComeOutRoll = 0;
    numCurCnsectvTwosOnComeOutRoll    = 0;
    numCurCnsectvThreesOnComeOutRoll  = 0;
    numCurCnsectvTwelvesOnComeOutRoll = 0;
    numCurCnsectvCrapsOnComeOutRoll   = 0;

    numMaxCnsectvSevensOnComeOutRoll  = 0;
    numMaxCnsectvElevensOnComeOutRoll = 0;
    numMaxCnsectvTwosOnComeOutRoll    = 0;
    numMaxCnsectvThreesOnComeOutRoll  = 0;
    numMaxCnsectvTwelvesOnComeOutRoll = 0;
    numMaxCnsectvCrapsOnComeOutRoll   = 0;

    numComeWinsOn4  = 0;
    numComeWinsOn5  = 0;
    numComeWinsOn6  = 0;
    numComeWinsOn8  = 0;
    numComeWinsOn9  = 0;
    numComeWinsOn10 = 0;

    numMaxCnsectvComeWinsOn4  = 0;
    numMaxCnsectvComeWinsOn5  = 0;
    numMaxCnsectvComeWinsOn6  = 0;
    numMaxCnsectvComeWinsOn8  = 0;
    numMaxCnsectvComeWinsOn9  = 0;
    numMaxCnsectvComeWinsOn10 = 0;

    numCurCnsectvComeWinsOn4  = 0;
    numCurCnsectvComeWinsOn5  = 0;
    numCurCnsectvComeWinsOn6  = 0;
    numCurCnsectvComeWinsOn8  = 0;
    numCurCnsectvComeWinsOn9  = 0;
    numCurCnsectvComeWinsOn10 = 0;

    numComeLossOn4  = 0;
    numComeLossOn5  = 0;
    numComeLossOn6  = 0;
    numComeLossOn8  = 0;
    numComeLossOn9  = 0;
    numComeLossOn10 = 0;

    numMaxCnsectvComeLossOn4  = 0;
    numMaxCnsectvComeLossOn5  = 0;
    numMaxCnsectvComeLossOn6  = 0;
    numMaxCnsectvComeLossOn8  = 0;
    numMaxCnsectvComeLossOn9  = 0;
    numMaxCnsectvComeLossOn10 = 0;

    numCurCnsectvComeLossOn4  = 0;
    numCurCnsectvComeLossOn5  = 0;
    numCurCnsectvComeLossOn6  = 0;
    numCurCnsectvComeLossOn8  = 0;
    numCurCnsectvComeLossOn9  = 0;
    numCurCnsectvComeLossOn10 = 0;

    numDontWinsOn4  = 0;
    numDontWinsOn5  = 0;
    numDontWinsOn6  = 0;
    numDontWinsOn8  = 0;
    numDontWinsOn9  = 0;
    numDontWinsOn10 = 0;

    numMaxCnsectvDontWinsOn4  = 0;
    numMaxCnsectvDontWinsOn5  = 0;
    numMaxCnsectvDontWinsOn6  = 0;
    numMaxCnsectvDontWinsOn8  = 0;
    numMaxCnsectvDontWinsOn9  = 0;
    numMaxCnsectvDontWinsOn10 = 0;

    numCurCnsectvDontWinsOn4  = 0;
    numCurCnsectvDontWinsOn5  = 0;
    numCurCnsectvDontWinsOn6  = 0;
    numCurCnsectvDontWinsOn8  = 0;
    numCurCnsectvDontWinsOn9  = 0;
    numCurCnsectvDontWinsOn10 = 0;

    numDontLossOn4  = 0;
    numDontLossOn5  = 0;
    numDontLossOn6  = 0;
    numDontLossOn8  = 0;
    numDontLossOn9  = 0;
    numDontLossOn10 = 0;

    numMaxCnsectvDontLossOn4  = 0;
    numMaxCnsectvDontLossOn5  = 0;
    numMaxCnsectvDontLossOn6  = 0;
    numMaxCnsectvDontLossOn8  = 0;
    numMaxCnsectvDontLossOn9  = 0;
    numMaxCnsectvDontLossOn10 = 0;

    numCurCnsectvDontLossOn4  = 0;
    numCurCnsectvDontLossOn5  = 0;
    numCurCnsectvDontLossOn6  = 0;
    numCurCnsectvDontLossOn8  = 0;
    numCurCnsectvDontLossOn9  = 0;
    numCurCnsectvDontLossOn10 = 0;

    numHardwayWinsOn4  = 0;
    numHardwayWinsOn6  = 0;
    numHardwayWinsOn8  = 0;
    numHardwayWinsOn10 = 0;

    numCurCnsectvHardwayWinsOn4  = 0;
    numCurCnsectvHardwayWinsOn6  = 0;
    numCurCnsectvHardwayWinsOn8  = 0;
    numCurCnsectvHardwayWinsOn10 = 0;

    numMaxCnsectvHardwayWinsOn4  = 0;
    numMaxCnsectvHardwayWinsOn6  = 0;
    numMaxCnsectvHardwayWinsOn8  = 0;
    numMaxCnsectvHardwayWinsOn10 = 0;

    numHardwayLossOn4  = 0;
    numHardwayLossOn6  = 0;
    numHardwayLossOn8  = 0;
    numHardwayLossOn10 = 0;

    numCurCnsectvHardwayLossOn4  = 0;
    numCurCnsectvHardwayLossOn6  = 0;
    numCurCnsectvHardwayLossOn8  = 0;
    numCurCnsectvHardwayLossOn10 = 0;

    numMaxCnsectvHardwayLossOn4  = 0;
    numMaxCnsectvHardwayLossOn6  = 0;
    numMaxCnsectvHardwayLossOn8  = 0;
    numMaxCnsectvHardwayLossOn10 = 0;

    num2  = 0;
    num3  = 0;
    num4  = 0;
    num5  = 0;
    num6  = 0;
    num7  = 0;
    num8  = 0;
    num9  = 0;
    num10 = 0;
    num11 = 0;
    num12 = 0;

    numCurCnsectvNum2  = 0;
    numCurCnsectvNum3  = 0;
    numCurCnsectvNum4  = 0;
    numCurCnsectvNum5  = 0;
    numCurCnsectvNum6  = 0;
    numCurCnsectvNum7  = 0;
    numCurCnsectvNum8  = 0;
    numCurCnsectvNum9  = 0;
    numCurCnsectvNum10 = 0;
    numCurCnsectvNum11 = 0;
    numCurCnsectvNum12 = 0;

    numMaxCnsectvNum2  = 0;
    numMaxCnsectvNum3  = 0;
    numMaxCnsectvNum4  = 0;
    numMaxCnsectvNum5  = 0;
    numMaxCnsectvNum6  = 0;
    numMaxCnsectvNum7  = 0;
    numMaxCnsectvNum8  = 0;
    numMaxCnsectvNum9  = 0;
    numMaxCnsectvNum10 = 0;
    numMaxCnsectvNum11 = 0;
    numMaxCnsectvNum12 = 0;
    
    numTurnsShooter               = 0;
    numCurCnsectvRollsThisShooter = 0;
    numMaxCnsectvRollsThisShooter = 0;
    totCnsectvRollsPerShooter     = 0;

    armed4  = false;
    armed5  = false;
    armed6  = false;
    armed7  = false;
    armed8  = false;
    armed9  = false;
    armed10 = false;
}

//-----------------------------------------------------------------
