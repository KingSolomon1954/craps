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
    unsigned roll = curRoll.value();
    unsigned d1   = curRoll.d1();
    unsigned d2   = curRoll.d2();

    countDiceNumbers     (curRoll, prevRoll);
    countComeOutRolls    (point);
    countPointRolls      (point, roll);
    countShooterRolls    (roll);
    countFieldBetWins    (roll);
    countFieldBetLose    (roll);
    countHardwayWins     (d1, d2);
    countHardwayLose     (d1, d2);
    
    switch(roll)
    {
        case 2:  update2 (point);  break;
        case 3:  update3 (point);  break;
        case 7:  update7 (point);  break;
        case 11: update11(point);  break;
        case 12: update12(point);  break;
        case 4:  // fall thru
        case 5:  // fall thru
        case 6:  // fall thru
        case 8:  // fall thru
        case 9:  // fall thru
        case 10: updatePointRoll(point, roll);  break;
    }
}

//-----------------------------------------------------------------

void
TableStats::countDiceNumbers(const Dice& curRoll,
                             const Dice& prevRoll)
{
    unsigned roll = curRoll.value();
    NumberCounts& nc = numberCounts[roll];
    
    nc.count++;
    if (roll == prevRoll.value())
    {
        nc.curCnsectvCount++;
        nc.maxCnsectvCount =
            std::max(nc.curCnsectvCount, nc.maxCnsectvCount);
    }
    else
    {
        nc.curCnsectvCount = 0;
    }
}

//----------------------------------------------------------------

void
TableStats::countComeOutRolls(unsigned point)
{
    if (point == 0)
    {
        numComeOutRolls++;
        numCurCnsectvComeOutRolls++;
        numMaxCnsectvComeOutRolls =
            std::max(numMaxCnsectvComeOutRolls, numCurCnsectvComeOutRolls);
        numCurCnsectvNonComeOutRolls = 0;
    }
}

//----------------------------------------------------------------

void
TableStats::countPointRolls(unsigned point, unsigned roll)
{
    if (point != 0 && roll != 7)
    {
        numNonComeOutRolls++;
        numCurCnsectvNonComeOutRolls++;
        numMaxCnsectvNonComeOutRolls =
            std::max(numMaxCnsectvNonComeOutRolls,
                     numCurCnsectvNonComeOutRolls);
        numCurCnsectvComeOutRolls = 0;
    }
}

//----------------------------------------------------------------

void
TableStats::countShooterRolls(unsigned roll)
{
    if (roll == 7)
    {
        numCurCnsectvRollsThisShooter = 0;
        return;
    }
    numCurCnsectvRollsThisShooter++;
    numMaxCnsectvRollsThisShooter = std::max(numMaxCnsectvRollsThisShooter,
                                             numCurCnsectvRollsThisShooter);
}

//-----------------------------------------------------------------

void
TableStats::countFieldBetWins(unsigned roll)
{
    if (fieldNums_.contains(roll))
    {
        numFieldBetWins++;
        numCurCnsectvFieldBetWins++;
        numMaxCnsectvFieldBetWins = std::max(numMaxCnsectvFieldBetWins,
                                             numCurCnsectvFieldBetWins);
        numCurCnsectvFieldBetLoss = 0;
    }
}

//-----------------------------------------------------------------

void
TableStats::countFieldBetLose(unsigned roll)
{
    if (!fieldNums_.contains(roll))
    {
        numFieldBetLoss++;
        numCurCnsectvFieldBetLoss++;
        numMaxCnsectvFieldBetLoss = std::max(numMaxCnsectvFieldBetLoss,
                                             numCurCnsectvFieldBetLoss);
        numCurCnsectvFieldBetWins = 0;
    }
}

//-----------------------------------------------------------------

void
TableStats::countHardwayWins(unsigned point, unsigned d1, unsigned d2)
{
    if (point == 0) return;  // Assume no hardways on come out rolls
    
    if (d1 == d2)
    {
        const unsigned roll = d1 + d2;
        if (roll == 4)
        {
            bumpHardwayWinsOn4();
        }
        if (roll == 6)
        {
            bumpHardwayWinsOn6();
        }
        if (roll == 8)
        {
            bumpHardwayWinsOn8();
        }
        if (roll == 10)
        {
            bumpHardwayWinsOn10();
        }
    }
}

//-----------------------------------------------------------------

void
TableStats::countHardwayLose(unsigned point, unsigned d1, unsigned d2)
{
    if (point == 0) return;  // Assume no hardways on come out rolls
    
    if (d1 != d2)
    {
        const unsigned roll = d1 + d2;
        if (roll == 4 || roll == 7) 
        {
            bumpHardwayLoseOn4();
        }
        if (roll == 6 || roll == 7)
        {
            bumpHardwayLoseOn6();
        }
        if (roll == 8 || roll == 7)
        {
            bumpHardwayLoseOn8();
        }
        if (roll == 10 || roll == 7)
        {
            bumpHardwayLoseOn10();
        }
    }
}

//----------------------------------------------------------------

void
TableStats::update2(unsigned point)
{
    if (point == 0)
    {
        numTwosOnComeOutRoll++;
        numCrapsOnComeOutRoll++;
        bumpPassLineLose();
        bumpDontPassWins();
    }
    else
    {
        bumpComeLose();
        bumpDontComeWins();
    }
}

//-----------------------------------------------------------------

void
TableStats::update3(unsigned point)
{
    if (point == 0)
    {
        numThreesOnComeOutRoll++;
        numCrapsOnComeOutRoll++;
        bumpPassLineLose();
        bumpDontPassWins();
    }
    else
    {
        bumpComeLose();
        bumpDontComeWins();
    }
}

//-----------------------------------------------------------------

void
TableStats::update7(unsigned point)
{
    if (point == 0)
    {
        bumpPassLineWins();
        bumpDontPassLose();
        bumpSevensOnComeOutRoll();
    }
    else
    {
        bumpComeWins();
        bumpDontComeLose();
        bumpSevenOuts();
    }

    countComeLose(0, 4);
    countComeLose(0, 5);
    countComeLose(0, 6);
    countComeLose(0, 8);
    countComeLose(0, 9);
    countComeLose(0, 10);

    countDontComeWins(0, 4);
    countDontComeWins(0, 5);
    countDontComeWins(0, 6);
    countDontComeWins(0, 8);
    countDontComeWins(0, 9);
    countDontComeWins(0, 10);
}

//-----------------------------------------------------------------

void
TableStats::update11(unsigned point)
{
    if (point == 0)
    {
        numElevensOnComeOutRoll++;
        bumpPassLineWins();
        bumpDontPassLose();
    }
    else
    {
        bumpComeWins();
        bumpDontComeLose();
    }
}

//-----------------------------------------------------------------

void
TableStats::update12(unsigned point)
{
    if (point == 0)
    {
        numTwelvesOnComeOutRoll++;
        numCrapsOnComeOutRoll++;
        bumpPassLineLose();
        // bumpDontPassWin();  // push
    }
    else
    {
        bumpComeLose();
        // bumpDontComeWins();  // push
    }
}

//-----------------------------------------------------------------

void
TableStats::updatePointRoll(unsigned point, unsigned roll)
{
    if (point == roll)
    {
        countPassLinewins(roll);
        countDontPassLose(roll);
    }
    countComeWins(point, roll);
    countDontComeLose(point, roll);
}

//----------------------------------------------------------------

void
TableStats::bumpPassLineWins()
{
    numPassLineWins++;
    numCurCnsectvPassLineWins++;
    numMaxCnsectvPassLineWins = std::max(numMaxCnsectvPassLineWins,
                                         numCurCnsectvPassLineWins);
    numCurCnsectvPassLineLose = 0;
}

//----------------------------------------------------------------

void
TableStats::bumpPassLineLose()
{
    numPassLineLose++;
    numCurCnsectvPassLineLose++;
    numMaxCnsectvPassLineLose = std::max(numMaxCnsectvPassLineLose,
                                         numCurCnsectvPassLineLose);
    numCurCnsectvPassLineWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpDontPassWins()
{
    numDontPassWins++;
    numCurCnsectvDontPassWins++;
    numMaxCnsectvDontPassWins = std::max(numMaxCnsectvDontPassWins,
                                         numCurCnsectvDontPassWins);
    numCurCnsectvDontPassLose = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpDontPassLose()
{
    numDontPassLose++;
    numCurCnsectvDontPassLose++;
    numMaxCnsectvDontPassLose = std::max(numMaxCnsectvDontPassLose,
                                         numCurCnsectvDontPassLose);
    numCurCnsectvDontPassWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpComeWins()
{
    numComeWins++;
    numCurCnsectvComeWins++;
    numMaxCnsectvComeWins = std::max(numCurCnsectvComeins,
                                         numMaxCnsectvComeWins);
    numCurCnsectvComeLose = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpComeLose()
{
    numComeLose++;
    numCurCnsectvComeLose++;
    numMaxCnsectvComeLose = std::max(numCurCnsectvComeLose,
                                     numMaxCnsectvComeLose);
    numCurCnsectvComeWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpDontComeWins()
{
    numDontComeWins++;
    numCurCnsectvDontComeWins++;
    numMaxCnsectvDontComeWins = std::max(numCurCnsectvDontComeins,
                                         numMaxCnsectvDontComeWins);
    numCurCnsectvComeLose = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpDontComeLose()
{
    numContComeLose++;
    numCurCnsectvDontComeLose++;
    numMaxCnsectvDontComeLose = std::max(numCurCnsectvDontComeLose,
                                         numMaxCnsectvDontComeLose);
    numCurCnsectvDontComeWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayWinsOn4()
{
    numHardwayWinsOn4++;
    numCurCnsectvHardwayWinsOn4++;
    numMaxCnsectvHardwayWinsOn4 = std::max(numMaxCnsectvHardwayWinsOn4,
                                           numCurCnsectvHardwayWinsOn4);
    numCurCnsectvHardwayLoseOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayLoseOn4()
{
    numHardwayLoseOn4++;
    numCurCnsectvHardwayLoseOn4++;
    numMaxCnsectvHardwayLoseOn4 = std::max(numMaxCnsectvHardwayLoseOn4,
                                           numCurCnsectvHardwayLoseOn4);
    numCurCnsectvHardwayWinsOn4 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayWinsOn6()
{
    numHardwayWinsOn6++;
    numCurCnsectvHardwayWinsOn6++;
    numMaxCnsectvHardwayWinsOn6 = std::max(numMaxCnsectvHardwayWinsOn6,
                                           numCurCnsectvHardwayWinsOn6);
    numCurCnsectvHardwayLoseOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayLoseOn6()
{
    numHardwayLoseOn6++;
    numCurCnsectvHardwayLoseOn6++;
    numMaxCnsectvHardwayLoseOn6 = std::max(numMaxCnsectvHardwayLoseOn6,
                                           numCurCnsectvHardwayLoseOn6);
    numCurCnsectvHardwayWinsOn6 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayWinsOn8()
{
    numHardwayWinsOn8++;
    numCurCnsectvHardwayWinsOn8++;
    numMaxCnsectvHardwayWinsOn8 = std::max(numMaxCnsectvHardwayWinsOn8,
                                           numCurCnsectvHardwayWinsOn8);
    numCurCnsectvHardwayLoseOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayLoseOn8()
{
    numHardwayLoseOn8++;
    numCurCnsectvHardwayLoseOn8++;
    numMaxCnsectvHardwayLoseOn8 = std::max(numMaxCnsectvHardwayLoseOn8,
                                           numCurCnsectvHardwayLoseOn8);
    numCurCnsectvHardwayWinsOn8 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayWinsOn10()
{
    numHardwayWinsOn10++;
    numCurCnsectvHardwayWinsOn10++;
    numMaxCnsectvHardwayWinsOn10 = std::max(numMaxCnsectvHardwayWinsOn10,
                                            numCurCnsectvHardwayWinsOn10);
    numCurCnsectvHardwayLoseOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayLoseOn10()
{
    numHardwayLossOn10++;
    numCurCnsectvHardwayLossOn10++;
    numMaxCnsectvHardwayLossOn10 = std::max(numMaxCnsectvHardwayLossOn10,
                                            numCurCnsectvHardwayLossOn10);
    numCurCnsectvHardwayWinsOn10 = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpSevensOnComeOutRoll()
{
    numSevensOnComeOutRoll++;
    numCurCnsectvSevensOnComeOutRoll++;
    numMaxCnsectvSevensOnComeOutRoll =
        std::max(numMaxCnsectvSevensOnComeOutRoll,
                 numCurCnsectvSevensOnComeOutRoll);
}

//----------------------------------------------------------------

void
TableStats::countPassLineWins(unsigned roll)
{
    bumpPassLineWins();
    // Update stats on the number itself
    PointCounts& pc = passLineCounts[roll];
    pc.numWins++;
    pc.curCnsectvWinsCount++;
    pc.maxCnsectvWinsCount =
        std::max(pc.curCnsectvWinsCount, pc.maxCnsectvWinsCount);
    pc.curCnsectvLossCount = 0;
}

//----------------------------------------------------------------

void
TableStats::countPassLineLose(unsigned roll)
{
    bumpPassLineLose();
    // Update stats on the number itself
    PointCounts& pc = passLineCounts[roll];
    pc.numLose++;
    pc.curCnsectvLoseCount++;
    pc.maxCnsectvLoseCount =
        std::max(pc.curCnsectvLoseCount, pc.maxCnsectvLoseCount);
    pc.curCnsectvWinsCount = 0;
}

//----------------------------------------------------------------

void
TableStats::countDontPassWins(unsigned roll)
{
    bumpDontPassWins();
    // Update stats on the number itself
    PointCounts& pc = dontPassCounts[roll];
    pc.numWins++;
    pc.curCnsectvWinsCount++;
    pc.maxCnsectvWinsCount =
        std::max(pc.curCnsectvWinsCount, pc.maxCnsectvWinsCount);
    pc.curCnsectvLossCount = 0;
}

//----------------------------------------------------------------

void
TableStats::countDontPassLose(unsigned roll)
{
    bumpDontPassLose();
    // Update stats on the number itself
    PointCounts& pc = dontPassCounts[roll];
    pc.numLose++;
    pc.curCnsectvLoseCount++;
    pc.maxCnsectvLoseCount =
        std::max(pc.curCnsectvLoseCount, pc.maxCnsectvLoseCount);
    pc.curCnsectvWinsCount = 0;
}

//----------------------------------------------------------------

void
TableStats::countComeWins(unsigned point, unsigned roll)
{
    if (comeCounts[roll].armed)  // Update per number stats
    {
        bumpComeWins();
        // Update stats on the number itself
        PointCounts& pc = comeCounts[roll];
        pc.numWins++;
        pc.curCnsectvWinsCount++;
        pc.maxCnsectvWinsCount =
            std::max(pc.curCnsectvWinsCount, pc.maxCnsectvWinsCount);
        pc.armed = false;
        pc.curCnsectvLossCount = 0;
    }

    // May need to arm the next come bet for this number
    if (point != 0)   // But only if not coming out
    {
        comeCounts[roll].armed = true;
    }
}

//----------------------------------------------------------------

void
TableStats::countComeLose(unsigned point, unsigned roll)
{
    if (comeCounts[roll].armed)
    {
        bumpComeLose();
        // Update stats on the number itself
        PointCounts& pc = comeCounts[roll];
        pc.numLose++;
        pc.curCnsectvLoseCount++;
        pc.maxCnsectvLoseCount =
            std::max(pc.curCnsectvLoseCount, pc.maxCnsectvLoseCount);
        pc.curCnsectvWinsCount = 0;
    }

    // May need to arm the next come bet for this number
    if (point != 0)   // But only if not coming out
    {
        comeCounts[roll].armed = true;
    }
}

//----------------------------------------------------------------

void
TableStats::countDontComeWins(unsigned point, unsigned roll)
{
    if (dontComeCounts[roll].armed)  // Update per number stats
    {
        bumpDontComeWins();
        // Update stats on the number itself
        PointCounts& pc = dontComeCounts[roll];
        pc.numWins++;
        pc.curCnsectvWinsCount++;
        pc.maxCnsectvWinsCount =
            std::max(pc.curCnsectvWinsCount, pc.maxCnsectvWinsCount);
        pc.armed = false;
        pc.curCnsectvLossCount = 0;
    }

    // May need to arm the next dont come bet for this number
    if (point != 0)   // But only if not coming out
    {
        dontComeCounts[roll].armed = true;
    }
}

//----------------------------------------------------------------

void
TableStats::countDontComeLose(unsigned point, unsigned roll)
{
    if (dontComeCounts[roll].armed)
    {
        bumpDontComeLose();
        // Update stats on the number itself
        PointCounts& pc = comeCounts[roll];
        pc.numLose++;
        pc.curCnsectvLoseCount++;
        pc.maxCnsectvLoseCount =
            std::max(pc.curCnsectvLoseCount, pc.maxCnsectvLoseCount);
        pc.curCnsectvWinsCount = 0;
    }

    // May need to arm the next dont come bet for this number
    if (point != 0)   // But only if not coming out
    {
        dontComeCounts[roll].armed = true;
    }
}

//----------------------------------------------------------------

void
TableStats::bumpSevenOuts()
{
    numSevenOuts++;
    // avgNumSevenOuts = (double)numSevenOuts / (double)numRolls;
    
    numTurnsShooter++;           // Same as sevenOut count
    totCnsectvRollsPerShooter += numCurCnsectvRollsThisShooter;
    numCurCnsectvRollsThisShooter = 0;
    // avgNumRollsPerShooter = (double)totCnsectvRollsPerShooter /
    //                         (double)numTurnsShooter;
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
    numBetsMade           = 0;
    numBetsWin            = 0;
    numBetsLose           = 0;
    numBetsKeep           = 0;
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
    numRolls                     = 0;
    numSevenOuts                 = 0;
    
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
    numCurCnsectvTwelvesOnComeOutRoll = 0;

    numMaxCnsectvSevensOnComeOutRoll  = 0;
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
}

//-----------------------------------------------------------------
