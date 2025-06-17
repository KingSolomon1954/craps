//----------------------------------------------------------------
//
// File: TableStats.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <craps/CrapsBet.h>
#include <algorithm>

using namespace Craps;

TableStats::TableStats(const std::string& tableIdArg)
    : tableId(tableIdArg)
{
}

/*-----------------------------------------------------------*//**

Update lots of stats after dice throw.

*/
void
TableStats::updateDiceRoll(unsigned point,
                           const Dice& curRoll,
                           const Dice& prevRoll)
{
    numRolls++;
    unsigned roll = curRoll.value();
    unsigned d1   = curRoll.d1();
    unsigned d2   = curRoll.d2();

    disarmThese          (point, roll);
    countDiceNumbers     (curRoll, prevRoll);
    countComeOutRolls    (point);
    countPointRolls      (point, roll);
    countShooterRolls    (point, roll);
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
        comeOutRolls.bump(); pointRolls.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::countPointRolls(unsigned point, unsigned roll)
{
    if (point != 0 && roll != 7)
    {
        pointRolls.bump(); comeOutRolls.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::countShooterRolls(unsigned point, unsigned roll)
{
    if (point != 0 && roll == 7) return;
    shooterCounts.bump();  // see update7 for shooterCounts.disarm()
}

//-----------------------------------------------------------------

void
TableStats::countFieldBetWins(unsigned roll)
{
    if (CrapsBet::fieldNums_.contains(roll))
    {
        fieldBetWins.bump(); fieldBetLose.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::countFieldBetLose(unsigned roll)
{
    if (!CrapsBet::fieldNums_.contains(roll))
    {
        fieldBetLose.bump(); fieldBetWins.disarm();
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

//-----------------------------------------------------------------

void
TableStats::disarmThese(unsigned point, unsigned roll)
{
    if (roll != 2)
    {
        twosOnComeOutRoll.disarm();
    }
    if (roll != 3)
    {
        threesOnComeOutRoll.disarm();
    }
    if (roll != 7)
    {
        sevensOnComeOutRoll.disarm();
    }
    if (roll != 11)
    {
        elevensOnComeOutRoll.disarm();
    }
    if (roll != 12)
    {
        twelvesOnComeOutRoll.disarm();
    }
    if (!CrapsBet::crapsNums_.contains(roll))
    {
        crapsOnComeOutRoll.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::update2(unsigned point)
{
    if (point == 0)
    {
        twosOnComeOutRoll.bump();  // see disarmThese() 
        crapsOnComeOutRoll.bump(); // see disarmThese() 
        passLineLose.bump(); passLineWins.disarm();
        dontPassWins.bump(); dontPassLose.disarm();
    }
    else
    {
        comeLose.bump(); comeWins.disarm();
        dontComeWins.bump(); dontComeLose.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::update3(unsigned point)
{
    if (point == 0)
    {
        threesOnComeOutRoll.bump(); // see disarmThese() 
        crapsOnComeOutRoll.bump();  // see disarmThese() 
        passLineLose.bump(); passLineWins.disarm();
        dontPassWins.bump(); dontPassLose.disarm();
    }
    else
    {
        comeLose.bump(); comeWins.disarm();
        dontComeWins.bump(); dontComeLose.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::update7(unsigned point)
{
    if (point == 0)
    {
        passLineWins.bump(); passLineLose.disarm();
        dontPassLose.bump(); dontPassWins.disarm();
        sevensOnComeOutRoll.bump(); // see disarmThese() 
    }
    else
    {
        comeWins.bump(); comeLose.disarm();
        dontComeLose.bump(); dontComeWins.disarm();
        sevenOuts.bump(); // see disarmThese() 
        shooterCounts.disarm();
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
        elevensOnComeOutRoll.bump();  // see disarmThese()
        passLineWins.bump(); passLineLose.disarm();
        dontPassLose.bump(); dontPassWins.disarm();
    }
    else
    {
        comeWins.bump(); comeLose.disarm();
        dontComeLose.bump(); dontComeWins.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::update12(unsigned point)
{
    if (point == 0)
    {
        twelvesOnComeOutRoll.bump(); // see disarmThese()
        crapsOnComeOutRoll.bump();   // see disarmThese()
        passLineLose.bump(); passLineWins.disarm();
        dontPassLose.disarm(); // dontPassWin.bump();  // push
    }
    else
    {
        comeLose.bump(); comeWins.disarm();
        dontComeLose.disarm(); // dontComeWins.bump(); // push
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
        sevenOuts.disarm();
    }
    countComeWins(point, roll);
    countDontComeLose(point, roll);
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

//----------------------------------------------------------------

void
TableStats::countPassLineWins(unsigned roll)
{
    passLineWins.bump(); passLineLose.disarm();
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
    passLineLose.bump(); passLineWins.disarm();
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
    dontPassWins.bump(); dontPassLose.disarm();
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
    dontPassLose.bump(); dontPassWins.disarm();
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
        comeWins.bump(); comeLose.disarm();
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
        comeLose.bump(); comeWins.disarm();
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
        dontComeWins.bump(); dontComeLose.disarm();
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
        dontComeLose.bump(); dontComeWins.disarm();
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
    numRolls = 0;
    comeOutRolls.reset();
    pointRolls.reset();
    passLineWins.reset();
    passLineLose.reset();
    dontPassWins.reset();
    dontPassLose.reset();
    comeWins.reset();
    comeLose.reset();
    dontComeWins.reset();
    dontComeLose.reset();
    fieldBetWins.reset();
    fieldBetLose.reset();
    sevenOuts.reset();
    shooterCounts.reset();
    twosOnComeOutRoll.reset();
    threesOnComeOutRoll.reset();
    sevensOnComeOutRoll.reset();
    elevensOnComeOutRoll.reset();
    twelvesOnComeOutRoll.reset();
    crapsOnComeOutRoll.reset();
}

//-----------------------------------------------------------------

unsigned
TableStats::Counter::getCount()
{
    return count;
}

//-----------------------------------------------------------------

unsigned
TableStats::Counter::getMax()
{
    return maxCnsectvCount;
}

//-----------------------------------------------------------------

void
TableStats::Counter::bump()
{
    count++;
    armed ? curCnsectvCount++ : armed = true;
    maxCnsectvCount = std::max(curCnsectvCount, maxCnsectvCount);
}

//-----------------------------------------------------------------

void
TableStats::Counter::disarm()
{
    armed = false;
    curCnsectvCount = 0;
}

//-----------------------------------------------------------------

void
TableStats::Counter::reset()
{
    count = 0;
    armed = false;
    curCnsectvCount = 0;
    maxCnsectvCount = 0;
}

//-----------------------------------------------------------------
