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
Counter::merge(const Counter& session)
{
    count_ += session.count_;
    maxRepeats = std::max(maxRepeats, session.maxRepeats);
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
PointCounts::merge(const PointCounts& sessionPc)
{
    wins.merge(sessionPc.wins);
    lose.merge(sessionPc.lose);
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

void
RollStats::merge(const RollStats& session)
{
    for (unsigned i = 4; i < 11; i++)
    {
        passPntCnts[i].merge(session.passPntCnts[i]);
        dontPassPntCnts[i].merge(session.dontPassPntCnts[i]);
        comePntCnts[i].merge(session.comePntCnts[i]);
        dontComePntCnts[i].merge(session.dontComePntCnts[i]);
        hardwayCounts[i].merge(session.hardwayCounts[i]);
        anyEstPntCnts[i].merge(session.anyEstPntCnts[i]);
    }
    
    for (unsigned i = 2; i < 12; i++)
    {
        numberCounts[i].merge(session.numberCounts[i]);
    }

    numRolls += session.numRolls;
    numPointsEstablished += session.numPointsEstablished;
    comeOutRolls.merge(session.comeOutRolls);
    pointRolls.merge(session.pointRolls);
    passWinsComeOut.merge(session.passWinsComeOut);
    passLoseComeOut.merge(session.passLoseComeOut);
    dontPassWinsComeOut.merge(session.dontPassWinsComeOut);
    dontPassLoseComeOut.merge(session.dontPassLoseComeOut);
    comeWinsComeOut.merge(session.comeWinsComeOut);
    comeLoseComeOut.merge(session.comeLoseComeOut);
    dontComeWinsComeOut.merge(session.dontComeWinsComeOut);
    dontComeLoseComeOut.merge(session.dontComeLoseComeOut);
    fieldBetWins.merge(session.fieldBetWins);
    fieldBetLose.merge(session.fieldBetLose);
    sevenOuts.merge(session.sevenOuts);
    shooterCounts.merge(session.shooterCounts);
    twosOnComeOutRoll.merge(session.twosOnComeOutRoll);
    threesOnComeOutRoll.merge(session.threesOnComeOutRoll);
    sevensOnComeOutRoll.merge(session.sevensOnComeOutRoll);
    elevensOnComeOutRoll.merge(session.elevensOnComeOutRoll);
    twelvesOnComeOutRoll.merge(session.twelvesOnComeOutRoll);
    crapsOnComeOutRoll.merge(session.crapsOnComeOutRoll);
}

//-----------------------------------------------------------------
