//----------------------------------------------------------------
//
// File: RollStatsStruct.cpp
//
//----------------------------------------------------------------

#include <craps/RollStatsStruct.h>

using namespace Craps;

//-----------------------------------------------------------------

unsigned
Counter::count() const
{
    return count_;
}

//-----------------------------------------------------------------

unsigned
Counter::repeats() const
{
    return maxRepeats;
}

//-----------------------------------------------------------------

void
Counter::bump()
{
    count_++;
    armed ? curRepeats++ : armed = true;
    maxRepeats = std::max(curRepeats, maxRepeats);
}

//-----------------------------------------------------------------

void
Counter::disarm()
{
    armed = false;
    curRepeats = 0;
}

//-----------------------------------------------------------------

void
Counter::reset()
{
    count_     = 0;
    armed      = false;
    curRepeats = 0;
    maxRepeats = 0;
    pivot      = 0;
}

//-----------------------------------------------------------------

void
PointCounts::reset()
{
    wins.reset();
    lose.reset();
}

//-----------------------------------------------------------------

void
RollStats::reset()
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
