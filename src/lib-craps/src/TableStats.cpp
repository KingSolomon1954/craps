//----------------------------------------------------------------
//
// File: TableStats.cpp
//
//----------------------------------------------------------------

#include <craps/TableStats.h>
#include <craps/CrapsBet.h>
#include <craps/EnumBetName.h>

using namespace Craps;

/*-----------------------------------------------------------*//**

Update lots of stats after dice throw.

*/
void
TableStats::recordDiceRoll(unsigned point, const Dice& dice)
{
    resetRollCounts();  // Clear some counters from previous roll
    rollStats.numRolls++;
    unsigned roll = dice.value();
    unsigned d1   = dice.d1();
    unsigned d2   = dice.d2();

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
    rollStats.numberCounts[roll].bump();
    for (unsigned i = 2; i < 13; i++)
    {
        if (i != roll)
        {
            rollStats.numberCounts[i].disarm();
        }
    }
}

//-----------------------------------------------------------------

void
TableStats::countAllPoints(unsigned roll)
{
    rollStats.numPointsEstablished++;
    rollStats.anyEstPntCnts[roll].bump();
    for (unsigned i = 4; i < 11; i++)
    {
        if (i != roll)
        {
            rollStats.anyEstPntCnts[i].disarm();
        }
    }
}

//----------------------------------------------------------------

void
TableStats::countComeOutRolls(unsigned point)
{
    if (point == 0)
    {
        rollStats.comeOutRolls.bump(); rollStats.pointRolls.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::countPointRolls(unsigned point, unsigned roll)
{
    if (point != 0)
    {
        rollStats.pointRolls.bump(); rollStats.comeOutRolls.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::countShooterRolls(unsigned point, unsigned roll)
{
    rollStats.shooterCounts.bump();  // see update7 for shooterCounts.disarm()
}

//-----------------------------------------------------------------

void
TableStats::countFieldBetWins(unsigned roll)
{
    if (CrapsBet::fieldNums_.contains(roll))
    {
        rollStats.fieldBetWins.bump(); rollStats.fieldBetLose.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::countFieldBetLose(unsigned roll)
{
    if (!CrapsBet::fieldNums_.contains(roll))
    {
        rollStats.fieldBetLose.bump(); rollStats.fieldBetWins.disarm();
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
        rollStats.twosOnComeOutRoll.disarm();
    }
    if (roll != 3)
    {
        rollStats.threesOnComeOutRoll.disarm();
    }
    if (roll != 7)
    {
        rollStats.sevensOnComeOutRoll.disarm();
    }
    if (roll != 11)
    {
        rollStats.elevensOnComeOutRoll.disarm();
    }
    if (roll != 12)
    {
        rollStats.twelvesOnComeOutRoll.disarm();
    }
    if (!CrapsBet::crapsNums_.contains(roll))
    {
        rollStats.crapsOnComeOutRoll.disarm();
    }
}

//----------------------------------------------------------------

void
TableStats::update2(unsigned point)
{
    if (point == 0)
    {
        rollStats.twosOnComeOutRoll.bump();  // see disarmSomeCounts()
        rollStats.crapsOnComeOutRoll.bump(); // see disarmSomeCounts()
        rollStats.passLoseComeOut.bump(); rollStats.passWinsComeOut.disarm();
        rollStats.dontPassWinsComeOut.bump(); rollStats.dontPassLoseComeOut.disarm();
    }
    else
    {
        rollStats.comeLoseComeOut.bump(); rollStats.comeWinsComeOut.disarm();
        rollStats.dontComeWinsComeOut.bump(); rollStats.dontComeLoseComeOut.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::update3(unsigned point)
{
    if (point == 0)
    {
        rollStats.threesOnComeOutRoll.bump(); // see disarmSomeCounts()
        rollStats.crapsOnComeOutRoll.bump();  // see disarmSomeCounts()
        rollStats.passLoseComeOut.bump(); rollStats.passWinsComeOut.disarm();
        rollStats.dontPassWinsComeOut.bump(); rollStats.dontPassLoseComeOut.disarm();
    }
    else
    {
        rollStats.comeLoseComeOut.bump(); rollStats.comeWinsComeOut.disarm();
        rollStats.dontComeWinsComeOut.bump(); rollStats.dontComeLoseComeOut.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::update7(unsigned point)
{
    if (point == 0)
    {
        rollStats.passWinsComeOut.bump();     rollStats.passLoseComeOut.disarm();
        rollStats.dontPassLoseComeOut.bump(); rollStats.dontPassWinsComeOut.disarm();
        rollStats.sevensOnComeOutRoll.bump(); // see disarmSomeCounts()
    }
    else
    {
        countPassLinePntLose(point);
        countDontPassPntWins(point);
        rollStats.comeWinsComeOut.bump();     rollStats.comeLoseComeOut.disarm();
        rollStats.dontComeLoseComeOut.bump(); rollStats.dontComeWinsComeOut.disarm();

        rollStats.sevenOuts.bump();           // see disarmSomeCounts()
        rollStats.shooterCounts.disarm();     rollStats.shooterCounts.count_ = 0;
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
        rollStats.elevensOnComeOutRoll.bump();  // see disarmSomeCounts()
        rollStats.passWinsComeOut.bump(); rollStats.passLoseComeOut.disarm();
        rollStats.dontPassLoseComeOut.bump(); rollStats.dontPassWinsComeOut.disarm();
    }
    else
    {
        rollStats.comeWinsComeOut.bump(); rollStats.comeLoseComeOut.disarm();
        rollStats.dontComeLoseComeOut.bump(); rollStats.dontComeWinsComeOut.disarm();
    }
}

//-----------------------------------------------------------------

void
TableStats::update12(unsigned point)
{
    if (point == 0)
    {
        rollStats.twelvesOnComeOutRoll.bump(); // see disarmSomeCounts()
        rollStats.crapsOnComeOutRoll.bump();   // see disarmSomeCounts()
        rollStats.passLoseComeOut.bump(); rollStats.passWinsComeOut.disarm();
        rollStats.dontPassLoseComeOut.disarm(); // dontPassWin.bump();  // push
    }
    else
    {
        rollStats.comeLoseComeOut.bump(); rollStats.comeWinsComeOut.disarm();
        rollStats.dontComeLoseComeOut.disarm(); // rollStats.dontComeWinsComeOut.bump(); // push
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
        rollStats.sevenOuts.disarm();
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
        rollStats.comePntCnts[roll].wins.pivot     = roll;
        rollStats.comePntCnts[roll].lose.pivot     = roll;
        rollStats.dontComePntCnts[roll].wins.pivot = roll;
        rollStats.dontComePntCnts[roll].lose.pivot = roll;
    }
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayWins(unsigned roll)
{
    PointCounts& pc = rollStats.hardwayCounts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//-----------------------------------------------------------------

void
TableStats::bumpHardwayLose(unsigned roll)
{
    PointCounts& pc = rollStats.hardwayCounts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countPassLinePntWins(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = rollStats.passPntCnts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//----------------------------------------------------------------

void
TableStats::countPassLinePntLose(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = rollStats.passPntCnts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countDontPassPntWins(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = rollStats.dontPassPntCnts[roll];
    pc.wins.bump(); pc.lose.disarm();
}

//----------------------------------------------------------------

void
TableStats::countDontPassPntLose(unsigned roll)
{
    // Update stats on the number itself
    PointCounts& pc = rollStats.dontPassPntCnts[roll];
    pc.lose.bump(); pc.wins.disarm();
}

//----------------------------------------------------------------

void
TableStats::countComePntWins(unsigned point, unsigned roll)
{
    if (rollStats.comePntCnts[roll].wins.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = rollStats.comePntCnts[roll];
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
    if (rollStats.comePntCnts[roll].lose.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = rollStats.comePntCnts[roll];
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
    if (rollStats.dontComePntCnts[roll].wins.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = rollStats.dontComePntCnts[roll];
        pc.wins.bump(); pc.lose.disarm();
        // Unassign pivot, 7 clears all dontcome bets, no following bet.
        pc.wins.pivot = 0;
    }
}

//----------------------------------------------------------------

void
TableStats::countDontComePntLose(unsigned point, unsigned roll)
{
    if (rollStats.dontComePntCnts[roll].lose.pivot == roll)
    {
        // Update stats on the number itself
        PointCounts& pc = rollStats.dontComePntCnts[roll];
        pc.lose.bump(); pc.wins.disarm();
        // pc.lose.pivot remains assigned so dontcome bet is re-upped.
    }
}

//----------------------------------------------------------------

void
TableStats::resetRollCounts()
{
    betStats.numBetsOneRoll.current     = 0;
    betStats.numBetsWinOneRoll.current  = 0;
    betStats.numBetsLoseOneRoll.current = 0;
    betStats.numBetsKeepOneRoll.current = 0;
    betStats.amtBetsOneRoll.current     = 0;
    betStats.amtBetsWinOneRoll.current  = 0;
    betStats.amtBetsLoseOneRoll.current = 0;
    betStats.amtBetsKeepOneRoll.current = 0;
}

/*-----------------------------------------------------------*//**

Record a winning bet.

*/
void
TableStats::recordWin(const CrapsBet& bet, Gen::Money amtWin)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    std::string betName = betStats.expandBetName(bet);
    betStats.betTypeStats.wins[betName].count++;
    betStats.betTypeStats.wins[betName].amountBet   += amtBet;
    betStats.betTypeStats.wins[betName].amount      += amtWin;
    betStats.betTypeStats.wins[betName].totDistance += bet.distance();

    betStats.totNumBetsAllBets++;
    betStats.totAmtAllBets += amtBet;

    betStats.totNumWinsAllBets++;
    betStats.numBetsWinOneRoll.total++;

    betStats.totAmtWinsAllBets += amtWin;
    betStats.amtBetsWinOneRoll.total += amtWin;

    betStats.numBetsWinOneRoll.current++;

    betStats.amtBetsWinOneRoll.current += amtWin;

    betStats.maxAmtWinOneBet       = std::max(amtWin, betStats.maxAmtWinOneBet);
    betStats.amtBetsWinOneRoll.max = std::max(betStats.amtBetsWinOneRoll.current, betStats.amtBetsWinOneRoll.max);
    betStats.numBetsWinOneRoll.max = std::max(betStats.numBetsWinOneRoll.current, betStats.numBetsWinOneRoll.max);
}

/*-----------------------------------------------------------*//**

Record a losing bet.

*/
void
TableStats::recordLose(const CrapsBet& bet, Gen::Money amtLose)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    std::string betName = betStats.expandBetName(bet);
    betStats.betTypeStats.lose[betName].count++;
    betStats.betTypeStats.lose[betName].amountBet   += amtBet;
    betStats.betTypeStats.lose[betName].amount      += amtLose;
    betStats.betTypeStats.lose[betName].totDistance += bet.distance();

    betStats.totNumBetsAllBets++;
    betStats.totAmtAllBets += amtBet;

    betStats.totNumLoseAllBets++;
    betStats.numBetsLoseOneRoll.total++;

    betStats.totAmtLoseAllBets += amtLose;
    betStats.amtBetsLoseOneRoll.total += amtLose;

    betStats.numBetsLoseOneRoll.current++;

    betStats.amtBetsLoseOneRoll.current += amtLose;

    betStats.maxAmtLoseOneBet       = std::max(amtLose, betStats.maxAmtLoseOneBet);
    betStats.amtBetsLoseOneRoll.max = std::max(betStats.amtBetsLoseOneRoll.current, betStats.amtBetsLoseOneRoll.max);
    betStats.numBetsLoseOneRoll.max = std::max(betStats.numBetsLoseOneRoll.current, betStats.numBetsLoseOneRoll.max);
}

/*-----------------------------------------------------------*//**

Record a keeping bet.

No need to count keeps. They will eventually win or lose.
But might want to track avg number of keeps per roll.

*/
void
TableStats::recordKeep(const CrapsBet& bet)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    // betStats.totNumBetsAllBets++;      // Don't incr here, counted when win/lose
    // betStats.totAmtAllBets += amtBet;  // Don't incr here, counted when win/lose
    betStats.totNumKeepAllBets++;
    betStats.numBetsKeepOneRoll.total++;

    betStats.totAmtKeepAllBets += amtBet;
    betStats.amtBetsKeepOneRoll.total += amtBet;

    betStats.numBetsKeepOneRoll.current++;

    betStats.amtBetsKeepOneRoll.current += amtBet;

    betStats.maxAmtKeepOneBet       = std::max(amtBet, betStats.maxAmtKeepOneBet);
    betStats.amtBetsKeepOneRoll.max = std::max(betStats.amtBetsKeepOneRoll.current, betStats.amtBetsKeepOneRoll.max);
    betStats.numBetsKeepOneRoll.max = std::max(betStats.numBetsKeepOneRoll.current, betStats.numBetsKeepOneRoll.max);
}

//-----------------------------------------------------------------
//
// Helper function to update common stats between recordWin/Lose/Keep
//
void
TableStats::recordCommon(Gen::Money amtBet)
{
    betStats.numBetsOneRoll.total++;
    betStats.numBetsOneRoll.current++;
    betStats.amtBetsOneRoll.current += amtBet;
    betStats.maxAmtBetOneBet    = std::max(amtBet, betStats.maxAmtBetOneBet);
    betStats.amtBetsOneRoll.max = std::max(betStats.amtBetsOneRoll.current, betStats.amtBetsOneRoll.max);
    betStats.numBetsOneRoll.max = std::max(betStats.numBetsOneRoll.current, betStats.numBetsOneRoll.max);
}

//-----------------------------------------------------------------

void
TableStats::recordDeposit(Gen::Money amount)
{
    moneyStats.amtWithdrawn += amount;
    moneyStats.numWithdrawals++;
}

//-----------------------------------------------------------------

void
TableStats::recordWithdrawal(Gen::Money amount)
{
    moneyStats.amtWithdrawn += amount;
    moneyStats.numWithdrawals++;
}

//-----------------------------------------------------------------

void
TableStats::recordRefill(Gen::Money amount)
{
    moneyStats.amtRefilled += amount;
    moneyStats.numRefills++;
}

//-----------------------------------------------------------------

void
TableStats::reset()
{
    betStats.reset();
    rollStats.reset();
    moneyStats.reset();
    // No sessionHistory.clear()
}

//-----------------------------------------------------------------

void
TableStats::merge(const TableStats& session)
{
    betStats.merge(session.betStats);
    rollStats.merge(session.rollStats);
    moneyStats.merge(session.moneyStats);
    // No sessionHistory.merge()
}

//-----------------------------------------------------------------

void
TableStats::toYAML(YAML::Node& node) const
{
    node["BetStats"]     = betStats.toYAML();
    node["RollStats"]    = rollStats.toYAML();
    node["MoneyStats"]   = moneyStats.toYAML();
    node["SessionStats"] = sessionHistory.toYAML();
}

//-----------------------------------------------------------------

void
TableStats::fromYAML(const YAML::Node& node)
{
    betStats.fromYAML      (node["BetStats"]);
    rollStats.fromYAML     (node["RollStats"]);
    moneyStats.fromYAML    (node["MoneyStats"]);
    sessionHistory.fromYAML(node["SessionStats"]);
}

//-----------------------------------------------------------------
