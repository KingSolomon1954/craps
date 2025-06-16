//----------------------------------------------------------------
//
// File: TableStats.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <craps/CrapsBet.h>
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
    countHardwayWins     (point, d1, d2);
    countHardwayLose     (point, d1, d2);
    
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
        numCurCnsectvPointRolls = 0;
    }
}

//----------------------------------------------------------------

void
TableStats::countPointRolls(unsigned point, unsigned roll)
{
    if (point != 0 && roll != 7)
    {
        numPointRolls++;
        numCurCnsectvPointRolls++;
        numMaxCnsectvPointRolls =
            std::max(numMaxCnsectvPointRolls,
                     numCurCnsectvPointRolls);
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
    if (CrapsBet::fieldNums_.contains(roll))
    {
        numFieldBetWins++;
        numCurCnsectvFieldBetWins++;
        numMaxCnsectvFieldBetWins = std::max(numMaxCnsectvFieldBetWins,
                                             numCurCnsectvFieldBetWins);
        numCurCnsectvFieldBetLose = 0;
    }
}

//-----------------------------------------------------------------

void
TableStats::countFieldBetLose(unsigned roll)
{
    if (!CrapsBet::fieldNums_.contains(roll))
    {
        numFieldBetLose++;
        numCurCnsectvFieldBetLose++;
        numMaxCnsectvFieldBetLose = std::max(numMaxCnsectvFieldBetLose,
                                             numCurCnsectvFieldBetLose);
        numCurCnsectvFieldBetWins = 0;
    }
}

//-----------------------------------------------------------------

void
TableStats::countHardwayWins(unsigned point, unsigned d1, unsigned d2)
{
    if (point == 0) return;  // Assume no hardways bet on come out rolls
    
    const unsigned roll = d1 + d2;
    if (d1 == d2 && CrapsBet::hardwayNums_.contains(roll))
    {
        bumpHardwayWins(roll);
    }
}

//-----------------------------------------------------------------

void
TableStats::countHardwayLose(unsigned point, unsigned d1, unsigned d2)
{
    if (point == 0) return;  // Assume no hardways bet on come out rolls

    const unsigned roll = d1 + d2;
    if (roll == 7)
    {
        bumpHardwayLose(4);
        bumpHardwayLose(6);
        bumpHardwayLose(8);
        bumpHardwayLose(10);
    }
    
    if (d1 != d2 && CrapsBet::hardwayNums_.contains(roll))
    {
        bumpHardwayLose(roll);
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
        countPassLineWins(roll);
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
    numMaxCnsectvComeWins = std::max(numCurCnsectvComeWins,
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
    numMaxCnsectvDontComeWins = std::max(numCurCnsectvDontComeWins,
                                         numMaxCnsectvDontComeWins);
    numCurCnsectvComeLose = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpDontComeLose()
{
    numDontComeLose++;
    numCurCnsectvDontComeLose++;
    numMaxCnsectvDontComeLose = std::max(numCurCnsectvDontComeLose,
                                         numMaxCnsectvDontComeLose);
    numCurCnsectvDontComeWins = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayWins(unsigned roll)
{
    PointCounts& pc = hardwayCounts[roll];
    pc.numWins++;
    pc.curCnsectvWinsCount++;
    pc.maxCnsectvWinsCount = std::max(pc.curCnsectvWinsCount,
                                      pc.maxCnsectvWinsCount);
    pc.curCnsectvLoseCount = 0;
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayLose(unsigned roll)
{
    PointCounts& pc = hardwayCounts[roll];
    pc.numLose++;
    pc.curCnsectvLoseCount++;
    pc.maxCnsectvLoseCount = std::max(pc.curCnsectvLoseCount,
                                      pc.maxCnsectvLoseCount);
    pc.curCnsectvWinsCount = 0;
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
    pc.curCnsectvLoseCount = 0;
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
    pc.curCnsectvLoseCount = 0;
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
        pc.curCnsectvLoseCount = 0;
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
        pc.curCnsectvLoseCount = 0;
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
    numRolls     = 0;
    numSevenOuts = 0;

    numComeOutRolls           = 0;
    numCurCnsectvComeOutRolls = 0;
    numMaxCnsectvComeOutRolls = 0;
    
    numPointRolls           = 0;
    numCurCnsectvPointRolls = 0;
    numMaxCnsectvPointRolls = 0;
    
    numPassLineWins = 0;
    numPassLineLose = 0;
    numDontPassWins = 0;
    numDontPassLose = 0;
    numComeWins     = 0;
    numComeLose     = 0;
    numDontComeWins = 0;
    numDontComeLose = 0;
    
    numFieldBetWins           = 0;
    numFieldBetLose           = 0;
    numCurCnsectvFieldBetWins = 0;
    numCurCnsectvFieldBetLose = 0;
    numMaxCnsectvFieldBetWins = 0;
    numMaxCnsectvFieldBetLose = 0;
    
    numCurCnsectvPassLineWins = 0;
    numCurCnsectvPassLineLose = 0;
    numCurCnsectvDontPassWins = 0;
    numCurCnsectvDontPassLose = 0;
    numCurCnsectvComeWins     = 0;
    numCurCnsectvComeLose     = 0;
    numCurCnsectvDontComeWins = 0;
    numCurCnsectvDontComeLose = 0;
   
    numMaxCnsectvPassLineWins = 0;
    numMaxCnsectvPassLineLose = 0;
    numMaxCnsectvDontPassWins = 0;
    numMaxCnsectvDontPassLose = 0;
    numMaxCnsectvComeWins     = 0;
    numMaxCnsectvComeLose     = 0;
    numMaxCnsectvDontComeWins = 0;
    numMaxCnsectvDontComeLose = 0;
    
    numSevensOnComeOutRoll            = 0;
    numMaxCnsectvSevensOnComeOutRoll  = 0;
    numCurCnsectvSevensOnComeOutRoll  = 0;
    
    numElevensOnComeOutRoll = 0;
    numTwosOnComeOutRoll    = 0;
    numThreesOnComeOutRoll  = 0;
    numTwelvesOnComeOutRoll = 0;
    numCrapsOnComeOutRoll   = 0;

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
    
    numHardwayLoseOn4  = 0;
    numHardwayLoseOn6  = 0;
    numHardwayLoseOn8  = 0;
    numHardwayLoseOn10 = 0;
    
    numCurCnsectvHardwayLoseOn4  = 0;
    numCurCnsectvHardwayLoseOn6  = 0;
    numCurCnsectvHardwayLoseOn8  = 0;
    numCurCnsectvHardwayLoseOn10 = 0;
    
    numMaxCnsectvHardwayLoseOn4  = 0;
    numMaxCnsectvHardwayLoseOn6  = 0;
    numMaxCnsectvHardwayLoseOn8  = 0;
    numMaxCnsectvHardwayLoseOn10 = 0;
    
    numTurnsShooter               = 0;
    numCurCnsectvRollsThisShooter = 0;
    numMaxCnsectvRollsThisShooter = 0;
    totCnsectvRollsPerShooter     = 0;
}

//-----------------------------------------------------------------
