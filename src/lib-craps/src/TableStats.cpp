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
    
    disarmSomeCounts (point, roll);
    countDiceNumbers (roll);
    countComeOutRolls(point);
    countPointRolls  (point, roll);
    countShooterRolls(point, roll);
    countFieldBetWins(roll);
    countFieldBetLose(roll);
    countHardwayWins (point, d1, d2);
    countHardwayLose (point, d1, d2);
    
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

//-----------------------------------------------------------------

void
TableStats::countAllPoints(unsigned roll)
{
    numPointsEstablished++;
    anyEstPntCnts[roll].bump();
    for (unsigned i = 4; i < 11; i++)
    {
        if (i != roll)
        {
            anyEstPntCnts[i].disarm();
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
    if (point != 0)
    {
        pointRolls.bump(); comeOutRolls.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::countShooterRolls(unsigned point, unsigned roll)
{
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
// Based on roll, some single throw counters need to cancel
// their arm'ing.
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
        passLoseComeOut.bump(); passWinsComeOut.disarm();
        dontPassWinsComeOut.bump(); dontPassLoseComeOut.disarm();
    }
    else
    {
        comeLoseComeOut.bump(); comeWinsComeOut.disarm();
        dontComeWinsComeOut.bump(); dontComeLoseComeOut.disarm();
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
        passLoseComeOut.bump(); passWinsComeOut.disarm();
        dontPassWinsComeOut.bump(); dontPassLoseComeOut.disarm();
    }
    else
    {
        comeLoseComeOut.bump(); comeWinsComeOut.disarm();
        dontComeWinsComeOut.bump(); dontComeLoseComeOut.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::update7(unsigned point)
{
    if (point == 0)
    {
        passWinsComeOut.bump();     passLoseComeOut.disarm();
        dontPassLoseComeOut.bump(); dontPassWinsComeOut.disarm();
        sevensOnComeOutRoll.bump(); // see disarmSomeCounts() 
    }
    else
    {
        countPassLinePntLose(point);
        countDontPassPntWins(point);
        comeWinsComeOut.bump();     comeLoseComeOut.disarm();
        dontComeLoseComeOut.bump(); dontComeWinsComeOut.disarm();
        
        sevenOuts.bump();           // see disarmSomeCounts() 
        shooterCounts.disarm();     shooterCounts.count_ = 0;
    }

    countComePntLose(0, 4);
    countComePntLose(0, 5);
    countComePntLose(0, 6);
    countComePntLose(0, 8);
    countComePntLose(0, 9);
    countComePntLose(0, 10);

    countDontComePntWins(0, 4);
    countDontComePntWins(0, 5);
    countDontComePntWins(0, 6);
    countDontComePntWins(0, 8);
    countDontComePntWins(0, 9);
    countDontComePntWins(0, 10);
}

//-----------------------------------------------------------------

void
TableStats::update11(unsigned point)
{
    if (point == 0)
    {
        elevensOnComeOutRoll.bump();  // see disarmSomeCounts()
        passWinsComeOut.bump(); passLoseComeOut.disarm();
        dontPassLoseComeOut.bump(); dontPassWinsComeOut.disarm();
    }
    else
    {
        comeWinsComeOut.bump(); comeLoseComeOut.disarm();
        dontComeLoseComeOut.bump(); dontComeWinsComeOut.disarm();
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
        passLoseComeOut.bump(); passWinsComeOut.disarm();
        dontPassLoseComeOut.disarm(); // dontPassWin.bump();  // push
    }
    else
    {
        comeLoseComeOut.bump(); comeWinsComeOut.disarm();
        dontComeLoseComeOut.disarm(); // dontComeWinsComeOut.bump(); // push
    }
}

//-----------------------------------------------------------------

void
TableStats::updatePointRoll(unsigned point, unsigned roll)
{
    if (point == roll)
    {
        countPassLinePntWins(roll);
        countDontPassPntLose(roll);
        sevenOuts.disarm();
    }
    countComePntWins(point, roll);
    countDontComePntLose(point, roll);
    if (point == 0)
    {
        countAllPoints(roll);
    }
    if (point != 0)
    {
        // Establish new come and dont come bets
        comePntCnts[roll].wins.pivot     = roll;
        comePntCnts[roll].lose.pivot     = roll;
        dontComePntCnts[roll].wins.pivot = roll;
        dontComePntCnts[roll].lose.pivot = roll;
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
TableStats::countPassLinePntWins(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = passPntCnts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//----------------------------------------------------------------

void
TableStats::countPassLinePntLose(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = passPntCnts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countDontPassPntWins(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = dontPassPntCnts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//----------------------------------------------------------------

void
TableStats::countDontPassPntLose(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = dontPassPntCnts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countComePntWins(unsigned point, unsigned roll)
{
    if (comePntCnts[roll].wins.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = comePntCnts[roll];
        pc.wins.bump(); pc.lose.disarm();
        // pc.wins.pivot remains assigned so come bet is re-upped.
    }
}

//----------------------------------------------------------------
//
// Only called when there's a seven. Point is always 0.
// Is called for 4,5,6,8,9,10.
//
void
TableStats::countComePntLose(unsigned point, unsigned roll)
{
    if (comePntCnts[roll].lose.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = comePntCnts[roll];
        pc.lose.bump(); pc.wins.disarm();
        // Unassign pivot, 7-out clears all come bets, no following come bet.
        pc.lose.pivot = 0;
    }
}

//----------------------------------------------------------------
//
// Only called when there's a seven. Point is always 0.
// Is called for 4,5,6,8,9,10.
//
void
TableStats::countDontComePntWins(unsigned point, unsigned roll)
{
    if (dontComePntCnts[roll].wins.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = dontComePntCnts[roll];
        pc.wins.bump(); pc.lose.disarm(); 
        // Unassign pivot, 7 clears all dontcome bets, no following bet.
        pc.wins.pivot = 0;
    }
}

//----------------------------------------------------------------

void
TableStats::countDontComePntLose(unsigned point, unsigned roll)
{
    if (dontComePntCnts[roll].lose.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = dontComePntCnts[roll];
        pc.lose.bump(); pc.wins.disarm();
        // pc.lose.pivot remains assigned so dontcome bet is re-upped.
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

//----------------------------------------------------------------
//
// Helper function to expand bet names with their pivot.
//
// PassLine --> PassLine6
// Come     --> Come8
// Hardway  --> Hardway4
//
std::string
TableStats::expandBetName(const CrapsBetIntfc& bet) const
{
    std::string betName = EnumBetName::toString(bet.betName());
    if (bet.pivot() != 0)
    {
        betName += std::to_string(bet.pivot());
    }
    return betName;
}

/*-----------------------------------------------------------*//**

Record a winning bet.

*/
void
TableStats::recordWin(CrapsBetIntfc* bet, Gbl::Money amtWin)
{
    unsigned amtBet = bet->contractAmount() + bet->oddsAmount();
    recordCommon(amtBet);

    std::string betName = expandBetName(*bet);
    betsWinLose.wins[betName].count++;
    betsWinLose.wins[betName].amountBet   += amtBet;
    betsWinLose.wins[betName].amount      += amtWin;
    betsWinLose.wins[betName].totDistance += bet->distance();
    
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
    unsigned amtBet = bet->contractAmount() + bet->oddsAmount();
    recordCommon(amtBet);
    
    std::string betName = expandBetName(*bet);
    betsWinLose.lose[betName].count++;
    betsWinLose.lose[betName].amountBet   += amtBet;
    betsWinLose.lose[betName].amount      += amtLose;
    betsWinLose.lose[betName].totDistance += bet->distance();
    
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
    maxAmtBetOneBet    = std::max(amtBet, maxAmtBetOneBet);
    amtBetsOneRoll.max = std::max(amtBetsOneRoll.current, amtBetsOneRoll.max);
    numBetsOneRoll.max = std::max(numBetsOneRoll.current, numBetsOneRoll.max);
}

//-----------------------------------------------------------------

void
TableStats::reset()
{
    for (unsigned i = 4; i < 11; i++)
    {
        passPntCnts[i].reset();
        dontPassPntCnts[i].reset();
        comePntCnts[i].reset();
        dontComePntCnts[i].reset();
        hardwayCounts[i].reset();
        anyEstPntCnts[i].reset();
    }
    
    for (unsigned i = 2; i < 12; i++)
    {
        numberCounts[i].reset();
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
    numPointsEstablished = 0;
    comeOutRolls.reset();
    pointRolls.reset();
    passWinsComeOut.reset();
    passLoseComeOut.reset();
    dontPassWinsComeOut.reset();
    dontPassLoseComeOut.reset();
    comeWinsComeOut.reset();
    comeLoseComeOut.reset();
    dontComeWinsComeOut.reset();
    dontComeLoseComeOut.reset();
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
    count_     = 0;
    armed      = false;
    curRepeats = 0;
    maxRepeats = 0;
    pivot      = 0;
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
