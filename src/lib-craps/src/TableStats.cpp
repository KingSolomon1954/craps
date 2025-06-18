//----------------------------------------------------------------
//
// File: TableStats.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <craps/CrapsBet.h>
#include <craps/EnumBetName.h>
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
TableStats::recordDiceRoll(unsigned point, const Dice& dice)
{
    resetRollCounts();  // Clear some counters from previous roll
    numRolls++;
    unsigned roll = dice.value();
    unsigned d1   = dice.d1();
    unsigned d2   = dice.d2();

    bumpRecentRolls(dice);
    
    disarmSomeCounts     (point, roll);
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
//    if (point != 0 && roll != 7)
    if (point != 0)
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
//
// Based on roll, some counters need to cancel their arm'ing.
//
void
TableStats::disarmSomeCounts(unsigned point, unsigned roll)
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
        twosOnComeOutRoll.bump();  // see disarmSomeCounts() 
        crapsOnComeOutRoll.bump(); // see disarmSomeCounts() 
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
        threesOnComeOutRoll.bump(); // see disarmSomeCounts() 
        crapsOnComeOutRoll.bump();  // see disarmSomeCounts() 
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
        sevensOnComeOutRoll.bump(); // see disarmSomeCounts() 
    }
    else
    {
        passLineLose.bump(); passLineWins.disarm();
        dontPassWins.bump(); dontPassLose.disarm();
        comeWins.bump();     comeLose.disarm();
        dontComeLose.bump(); dontComeWins.disarm();
        sevenOuts.bump();    // see disarmSomeCounts() 
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
        elevensOnComeOutRoll.bump();  // see disarmSomeCounts()
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
        twelvesOnComeOutRoll.bump(); // see disarmSomeCounts()
        crapsOnComeOutRoll.bump();   // see disarmSomeCounts()
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

//----------------------------------------------------------------

void
TableStats::resetRollCounts()
{
    numBetsOneRoll.current     = 0;
    numBetsWinOneRoll.current  = 0;
    numBetsLoseOneRoll.current = 0;
    numBetsKeepOneRoll.current = 0;
    amtBetsOneRoll.current     = 0;
    amtBetsWinOneRoll.current  = 0;
    amtBetsLoseOneRoll.current = 0;
    amtBetsKeepOneRoll.current = 0;
}

/*-----------------------------------------------------------*//**

Record a winning bet.

*/
void
TableStats::recordWin(CrapsBetIntfc* bet, Gbl::Money amtWin)
{
    namespace e = EnumBetName;
    BetName betName = bet->betName();
    unsigned distance = bet->distance();
    betsWinLose.wins[e::toString(betName)].count++;
    betsWinLose.wins[e::toString(betName)].distance += distance;
    
    unsigned amtBet = bet->contractAmount() + bet->oddsAmount();
    recordCommon(amtBet);
    
    totNumBetsAllBets++;
    totAmtAllBets += amtBet;
    
    totNumWinsAllBets++;
    numBetsWinOneRoll.total++;
    
    totAmtWinsAllBets += amtWin;
    amtBetsWinOneRoll.total += amtWin;
    
    numBetsWinOneRoll.current++;
    
    amtBetsWinOneRoll.current += amtWin;
    
    maxAmtWinOneBet       = std::max(amtWin, maxAmtWinOneBet);
    amtBetsWinOneRoll.max = std::max(amtBetsWinOneRoll.current, amtBetsWinOneRoll.max);
    numBetsWinOneRoll.max = std::max(numBetsWinOneRoll.current, numBetsWinOneRoll.max);
}

/*-----------------------------------------------------------*//**

Record a losing bet.

*/
void
TableStats::recordLose(CrapsBetIntfc* bet, Gbl::Money amtLose)
{
    namespace e = EnumBetName;
    BetName betName = bet->betName();
    unsigned distance = bet->distance();
    betsWinLose.lose[e::toString(betName)].count++;
    betsWinLose.lose[e::toString(betName)].distance += distance;
    
    unsigned amtBet = bet->contractAmount() + bet->oddsAmount();
    recordCommon(amtBet);
    
    totNumBetsAllBets++;
    totAmtAllBets += amtBet;
    
    totNumLoseAllBets++;
    numBetsLoseOneRoll.total++;

    totAmtLoseAllBets += amtLose;
    amtBetsLoseOneRoll.total += amtLose;
    
    numBetsLoseOneRoll.current++;
        
    amtBetsLoseOneRoll.current += amtLose;

    maxAmtLoseOneBet       = std::max(amtLose, maxAmtLoseOneBet);
    amtBetsLoseOneRoll.max = std::max(amtBetsLoseOneRoll.current, amtBetsLoseOneRoll.max);
    numBetsLoseOneRoll.max = std::max(numBetsLoseOneRoll.current, numBetsLoseOneRoll.max);
}

/*-----------------------------------------------------------*//**

Record a keeping bet.

No need to count keeps. They will eventually win or lose.
But might want to track avg number of keeps per roll.

*/
void
TableStats::recordKeep(CrapsBetIntfc* bet)
{
    unsigned amtBet = bet->contractAmount() + bet->oddsAmount();
    recordCommon(amtBet);
    
    // totNumBetsAllBets++;      // Don't incr here, counted when win/lose
    // totAmtAllBets += amtBet;  // Don't incr here, counted when win/lose
    totNumKeepAllBets++;
    numBetsKeepOneRoll.total++;
    
    totAmtKeepAllBets += amtBet;
    amtBetsKeepOneRoll.total += amtBet;
    
    numBetsKeepOneRoll.current++;
    
    amtBetsKeepOneRoll.current += amtBet;

    maxAmtKeepOneBet       = std::max(amtBet, maxAmtKeepOneBet);
    amtBetsKeepOneRoll.max = std::max(amtBetsKeepOneRoll.current, amtBetsKeepOneRoll.max);
    numBetsKeepOneRoll.max = std::max(numBetsKeepOneRoll.current, numBetsKeepOneRoll.max);
}

//-----------------------------------------------------------------
//
// Helper function to update common stats between recordWin/Lose/Keep
//
void
TableStats::recordCommon(Gbl::Money amtBet)
{
    numBetsOneRoll.total++;
    numBetsOneRoll.current++;
    amtBetsOneRoll.current += amtBet;
    maxAmtBetOneBet   = std::max(amtBet, maxAmtBetOneBet);
    amtBetsOneRoll.max = std::max(amtBetsOneRoll.current, amtBetsOneRoll.max);
    numBetsOneRoll.max = std::max(numBetsOneRoll.current, numBetsOneRoll.max);
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
    totNumBetsAllBets = 0;
    totNumWinsAllBets = 0;
    totNumLoseAllBets = 0;
    totNumKeepAllBets = 0;
    totAmtAllBets     = 0;
    totAmtWinsAllBets = 0;
    totAmtLoseAllBets = 0;
    totAmtKeepAllBets = 0;
    maxAmtBetOneBet   = 0;
    maxAmtWinOneBet   = 0;
    maxAmtLoseOneBet  = 0;
    maxAmtKeepOneBet  = 0;
    numBetsOneRoll.reset();
    numBetsWinOneRoll.reset();
    numBetsLoseOneRoll.reset();
    numBetsKeepOneRoll.reset();
    amtBetsOneRoll.reset();
    amtBetsWinOneRoll.reset();
    amtBetsLoseOneRoll.reset();
    amtBetsKeepOneRoll.reset();

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

void
TableStats::bumpRecentRolls(const Dice& dice)
{
    if (recentRolls.size() >= rollHistorySize_)
    {
        recentRolls.pop_front();
    }
    recentRolls.push_back(dice);
}
    
//-----------------------------------------------------------------

void
TableStats::setRollHistorySize(size_t rollHistorySize)
{
    rollHistorySize_ = rollHistorySize;
}

//-----------------------------------------------------------------

size_t
TableStats::getRollHistorySize() const
{
    return rollHistorySize_;
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

void
TableStats::NumBets::reset()
{
    current = 0;
    max     = 0;
    total   = 0;
}

//-----------------------------------------------------------------

void
TableStats::AmtBets::reset()
{
    current = 0;
    max     = 0;
    total   = 0;
}

//-----------------------------------------------------------------
