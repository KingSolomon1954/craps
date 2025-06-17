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
TableStats::recordDiceRoll(unsigned point, const Dice& dRoll)
{
    numRolls++;
    unsigned roll = dRoll.value();
    unsigned d1   = dRoll.d1();
    unsigned d2   = dRoll.d2();

    disarmThese          (point, roll);
    countDiceNumbers     (roll);
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
TableStats::countDiceNumbers(unsigned roll)
{
    numberCounts[roll].bump();
    for (unsigned i = 2; i < 13; i++)
    {
        if (i != roll)
        {
            numberCounts[i].disarm();
        }
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
    if (point != 0)
    {
        // Establish new come and dont come bets
        comeCounts[roll].wins.armed = true;
        dontComeCounts[roll].wins.armed = true;
    }
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayWins(unsigned roll)
{
    PointCounts& pc = hardwayCounts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayLose(unsigned roll)
{
    PointCounts& pc = hardwayCounts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countPassLineWins(unsigned roll)
{
    passLineWins.bump(); passLineLose.disarm();
    // Update stats on the number itself
    PointCounts& pc = passLineCounts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//----------------------------------------------------------------

void
TableStats::countPassLineLose(unsigned roll)
{
    passLineLose.bump(); passLineWins.disarm();
    // Update stats on the number itself
    PointCounts& pc = passLineCounts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countDontPassWins(unsigned roll)
{
    dontPassWins.bump(); dontPassLose.disarm();
    // Update stats on the number itself
    PointCounts& pc = dontPassCounts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//----------------------------------------------------------------

void
TableStats::countDontPassLose(unsigned roll)
{
    dontPassLose.bump(); dontPassWins.disarm();
    // Update stats on the number itself
    PointCounts& pc = dontPassCounts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countComeWins(unsigned point, unsigned roll)
{
    if (comeCounts[roll].wins.armed)
    {
        comeWins.bump(); comeLose.disarm();
        // Update stats on the number itself
        PointCounts& pc = comeCounts[roll];
        pc.wins.bump(); pc.lose.disarm();
    }
}

//----------------------------------------------------------------
//
// Only called when there's a seven. Point is always 0.
// Is called for 4,5,6,8,9,10.
//
void
TableStats::countComeLose(unsigned point, unsigned roll)
{
    if (comeCounts[roll].wins.armed)
    {
        comeLose.bump(); comeWins.disarm();
        // Update stats on the number itself
        PointCounts& pc = comeCounts[roll];
        pc.lose.bump(); pc.wins.disarm();
        pc.lose.disarm();
    }
}

//----------------------------------------------------------------
//
// Only called when there's a seven. Point is always 0.
// Is called for 4,5,6,8,9,10.
//
void
TableStats::countDontComeWins(unsigned point, unsigned roll)
{
    if (dontComeCounts[roll].wins.armed)
    {
        dontComeWins.bump(); dontComeLose.disarm();
        // Update stats on the number itself
        PointCounts& pc = dontComeCounts[roll];
        pc.wins.bump(); pc.lose.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::countDontComeLose(unsigned point, unsigned roll)
{
    if (dontComeCounts[roll].wins.armed)
    {
        dontComeLose.bump(); dontComeWins.disarm();
        // Update stats on the number itself
        PointCounts& pc = dontComeCounts[roll];
        pc.lose.bump(); pc.wins.disarm();
    }
}

/*-----------------------------------------------------------*//**

Record winning bet.

*/
void
TableStats::recordWin(BetName betName, unsigned pivot,
                      Gbl::Money amtBet, Gbl::Money amtWin)
{
    toNumBetsAllBets++;
    toNumWinsAllBets++;
    totNumBetsOneRoll++;
    totNumBetsWinOneRoll++;

    totAmtAllBets    += amtBet;
    totAmtWinAllBets += amtWin;
    totAmtWinOneRoll += amtWin;
    
    curNumBetsOneRoll++;
    curNumBetsWinOneRoll++;
    
    curAmtBetsOneRoll += amtBet;
    curAmtWinOneRoll  += amtWin;
    
    maxAmtBetOneBet      = std::max(amtBet, maxAmtBetOneBet);
    maxAmtBetsOneRoll    = std::max(curAmtBetsOneRoll, maxAmtBetsOneRoll);
    maxAmtWinOneBet      = std::max(amtWin, maxAmtWinOneBet);
    maxAmtWinOneRoll     = std::max(curAmtWinOneRoll, maxAmtWinOneRoll);
    maxNumBetsOneRoll    = std::max(curNumBetsOneRoll, maxNumBetsOneRoll);
    maxNumBetsWinOneRoll = std::max(curNumBetsWinOneRoll, maxNumBetsWinOneRoll);
    // avgAmtPerBet = (double)totAmtAllBets / (double)numBetsMade;
}

/*-----------------------------------------------------------*//**

Record losing bet.

*/
void
TableStats::recordLose(BetName betName, unsigned pivot,
                       Gbl::Money amtBet, Gbl::Money amtLose)
{
    toNumBetsAllBets++;
    toNumLoseAllBets++;
    totNumBetsOneRoll++;
    totNumBetsLoseOneRoll++;

    totAmtAllBets     += amtBet;
    totAmtLoseAllBets += amtLose;
    totAmtLoseOneRoll += amtLose;
    
    curNumBetsOneRoll++;
    curNumBetsLoseOneRoll++;
        
    curAmtBetsOneRoll += amtBet;
    curAmtLoseOneRoll += amtLose;

    maxAmtBetOneBet       = std::max(amtBet, maxAmtBetOneBet);
    maxAmtBetsOneRoll     = std::max(curAmtBetsOneRoll, maxAmtBetsOneRoll);
    maxAmtLoseOneBet      = std::max(amtLose, maxAmtLoseOneBet);
    maxAmtLoseOneRoll     = std::max(curAmtLoseOneRoll, maxAmtLoseOneRoll);
    maxNumBetsOneRoll     = std::max(curNumBetsOneRoll, maxNumBetsOneRoll);
    maxNumBetsLoseOneRoll = std::max(curNumBetsLoseOneRoll, maxNumBetsLoseOneRoll);
}

/*-----------------------------------------------------------*//**

Record a keeping bet.

No need to count keeps. They will eventually win or lose.
But might want to track avg number of keeps per roll.

*/
void
TableStats::recordKeep(BetName betName, unsigned pivot, Gbl::Money amtBet)
{
    // toNumBetsAllBets++;  // Don't count here, will be counted when win/lose
    totNumKeepAllBets++;
    totNumBetsOneRoll++;
    totNumBetsKeepOneRoll++;
    
    totAmtAllBets     += amtBet;
    totAmtKeepAllBets += amtBet;
    totAmtKeepOneRoll += amtBet;
    
    curNumBetsOneRoll++;
    curNumBetsKeepOneRoll++;
    
    curAmtBetsOneRoll += amtBet;
    curAmtKeepOneRoll += amtBet;

    maxAmtBetOneBet       = std::max(amtBet, maxAmtBetOneBet);
    maxAmtBetsOneRoll     = std::max(curAmtBetsOneRoll, maxAmtBetsOneRoll);
    maxAmtKeepOneBet      = std::max(amtBet, maxAmtKeepOneBet);
    maxAmtKeepOneRoll     = std::max(curAmtKeepOneRoll, maxAmtKeepOneRoll);
    maxNumBetsOneRoll     = std::max(curNumBetsOneRoll, maxNumBetsOneRoll);
    maxNumBetsKeepOneRoll = std::max(curNumBetsKeepOneRoll, maxNumBetsKeepOneRoll);
}

//-----------------------------------------------------------------

void
TableStats::reset()
{
    for (unsigned i = 4; i < 11; i++)
    {
        passLineCounts[i].reset();
        dontPassCounts[i].reset();
        comeCounts[i].reset();
        dontComeCounts[i].reset();
        hardwayCounts[i].reset();
    }
    
    // Betting Stats
    toNumBetsAllBets      = 0;
    toNumWinsAllBets      = 0;
    toNumLoseAllBets      = 0;
    totNumKeepAllBets     = 0;
    totAmtAllBets         = 0;
    totAmtWinAllBets      = 0;
    totAmtLoseAllBets     = 0;
    totAmtKeepAllBets     = 0;
    maxAmtBetOneBet       = 0;
    maxAmtWinOneBet       = 0;
    maxAmtLoseOneBet      = 0;
    maxAmtKeepOneBet      = 0;
    curNumBetsOneRoll     = 0;
    maxNumBetsOneRoll     = 0;
    totNumBetsOneRoll     = 0;
    curNumBetsWinOneRoll  = 0;
    maxNumBetsWinOneRoll  = 0;
    totNumBetsWinOneRoll  = 0;
    curNumBetsLoseOneRoll = 0;
    maxNumBetsLoseOneRoll = 0;
    totNumBetsLoseOneRoll = 0;
    curNumBetsKeepOneRoll = 0;
    maxNumBetsKeepOneRoll = 0;
    totNumBetsKeepOneRoll = 0;
    curAmtBetsOneRoll     = 0;
    maxAmtBetsOneRoll     = 0;
    totAmtBetsOneRoll     = 0;
    curAmtWinOneRoll      = 0;
    maxAmtWinOneRoll      = 0;
    totAmtWinOneRoll      = 0;
    curAmtLoseOneRoll     = 0;
    maxAmtLoseOneRoll     = 0;
    totAmtLoseOneRoll     = 0;
    curAmtKeepOneRoll     = 0;
    maxAmtKeepOneRoll     = 0;
    totAmtKeepOneRoll     = 0;


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
TableStats::Counter::count() const
{
    return count_;
}

//-----------------------------------------------------------------

unsigned
TableStats::Counter::repeats() const
{
    return maxRepeats;
}

//-----------------------------------------------------------------

void
TableStats::Counter::bump()
{
    count_++;
    armed ? curRepeats++ : armed = true;
    maxRepeats = std::max(curRepeats, maxRepeats);
}

//-----------------------------------------------------------------

void
TableStats::Counter::disarm()
{
    armed = false;
    curRepeats = 0;
}

//-----------------------------------------------------------------

void
TableStats::Counter::reset()
{
    count_ = 0;
    armed = false;
    curRepeats = 0;
    maxRepeats = 0;
}

//-----------------------------------------------------------------

void
TableStats::PointCounts::reset()
{
    wins.reset();
    lose.reset();
}

//-----------------------------------------------------------------
