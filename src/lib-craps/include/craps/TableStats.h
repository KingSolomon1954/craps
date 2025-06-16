//----------------------------------------------------------------
//
// File: TableStats.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <string>
#include <craps/Dice.h>
#include <controller/Globals.h>
#include <gen/Timepoint.h>

namespace Craps {

class TableStats
{
public:

    struct NumberCounts
    {
        unsigned count = 0;
        unsigned curCnsectvCount = 0;
        unsigned maxCnsectvCount = 0;
    };
    std::array<NumberCounts, 13> numberCounts {};  // Index 2-12 used

    struct PointCounts
    {
        unsigned numWins = 0;
        unsigned numLose = 0;
        bool armed = false;
        unsigned curCnsectvWinsCount = 0;
        unsigned maxCnsectvWinsCount = 0;
        unsigned curCnsectvLoseCount = 0;
        unsigned maxCnsectvLoseCount = 0;
    };
    std::array<PointCounts, 11> passLineCounts {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> dontPassCounts {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> comeCounts     {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> dontComeCounts {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> hardwayCounts  {};  // Index 4,6,8,10 used
    
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    void updateAddBet(Gbl::Money betAmount);
    void updateOddsBet(Gbl::Money contractAmount, Gbl::Money oddsAmount);
    void updateBetsAfterThrow(Gbl::Money amtOnTable,
                              const std::pair<unsigned, Gbl::Money>& winStats,
                              const std::pair<unsigned, Gbl::Money>& loseStats,
                              unsigned numKeeps);
    void updateDiceRoll(unsigned point, const Dice& curRoll, const Dice& prevRoll);
    void reset();
    /// @}

    /// @name Observers
    /// @{
    /// @}

    std::string tableId;
    Gen::Timepoint sessionStart;
    Gen::Timepoint sessionEnd;
    
    // Betting Stats
    
    unsigned numBetsMade = 0;
    unsigned numBetsWin  = 0;
    unsigned numBetsLose = 0;
    unsigned numBetsKeep = 0;

    // double winPctg =  (numBetsWin  / numBetsMade) * 100
    // double losePctg = (numBetsLose / numBetsMade) * 100
    // double pushPctg = (numBetsPush / numBetsMade) * 100

    unsigned maxNumBetsPerRoll      = 0;
    unsigned totNumBetsPerRoll      = 0;
    // double avgNumBetsPerRoll     = 0.0;  // Up to user to calculate

    unsigned maxNumBetsWinPerRoll   = 0;
    unsigned totNumBetsWinPerRoll   = 0;
    // double avgNumBetsWinPerRoll  = 0.0;  // Up to user to calculate

    unsigned maxNumBetsLosePerRoll  = 0;
    unsigned totNumBetsLosePerRoll  = 0;
    // double avgNumBetsLosePerRoll = 0.0;  // Up to user to calculate

    unsigned maxNumBetsKeepPerRoll  = 0;
    unsigned totNumBetsKeepPerRoll  = 0;
    // double avgNumBetsKeepPerRoll = 0.0;  // Up to user to calculate

    Gbl::Money maxAmtOneBet         = 0;
    Gbl::Money totAmtAllBets        = 0;
    // double avgAmtPerBet          = 0.0;  // Up to user to calculate

    Gbl::Money maxAmtBetOneRoll     = 0;
    Gbl::Money totAmtBetPerRoll     = 0;
    // double avgAmtBetPerRoll      = 0.0;  // Up to user to calculate

    Gbl::Money maxAmtWinOneRoll     = 0;
    Gbl::Money totAmtWinPerRoll     = 0;
    // double avgAmtWinPerRoll      = 0.0;  // Up to user to calculate

    Gbl::Money maxAmtLoseOneRoll    = 0;
    Gbl::Money totAmtLosePerRoll    = 0;
    // double avgAmtLosePerRoll     = 0.0;  // Up to user to calculate

    Gbl::Money totAmtWin            = 0;
    Gbl::Money totAmtLose           = 0;
    // balance = totAmtWin - totAmtLose;    // Up to user to calculate
    
    // Dice Roll Stats
    
    unsigned numRolls     = 0;
    unsigned numSevenOuts = 0;              // Number of times dice passed.

    unsigned numComeOutRolls           = 0;
    unsigned numCurCnsectvComeOutRolls = 0;
    unsigned numMaxCnsectvComeOutRolls = 0;
    
    unsigned numPointRolls           = 0;
    unsigned numCurCnsectvPointRolls = 0;
    unsigned numMaxCnsectvPointRolls = 0;
    
    unsigned numPassLineWins = 0;
    unsigned numPassLineLose = 0;
    unsigned numDontPassWins = 0;
    unsigned numDontPassLose = 0;
    unsigned numComeWins     = 0;
    unsigned numComeLose     = 0;
    unsigned numDontComeWins = 0;
    unsigned numDontComeLose = 0;
    
    unsigned numFieldBetWins           = 0;
    unsigned numFieldBetLose           = 0;
    unsigned numCurCnsectvFieldBetWins = 0;
    unsigned numCurCnsectvFieldBetLose = 0;
    unsigned numMaxCnsectvFieldBetWins = 0;
    unsigned numMaxCnsectvFieldBetLose = 0;
    
    unsigned numCurCnsectvPassLineWins = 0;
    unsigned numCurCnsectvPassLineLose = 0;
    unsigned numCurCnsectvDontPassWins = 0;
    unsigned numCurCnsectvDontPassLose = 0;
    unsigned numCurCnsectvComeWins     = 0;
    unsigned numCurCnsectvComeLose     = 0;
    unsigned numCurCnsectvDontComeWins = 0;
    unsigned numCurCnsectvDontComeLose = 0;
   
    unsigned numMaxCnsectvPassLineWins = 0;
    unsigned numMaxCnsectvPassLineLose = 0;
    unsigned numMaxCnsectvDontPassWins = 0;
    unsigned numMaxCnsectvDontPassLose = 0;
    unsigned numMaxCnsectvComeWins     = 0;
    unsigned numMaxCnsectvComeLose     = 0;
    unsigned numMaxCnsectvDontComeWins = 0;
    unsigned numMaxCnsectvDontComeLose = 0;
    
    unsigned numSevensOnComeOutRoll            = 0; // How often 7 appeared on come out.
    unsigned numMaxCnsectvSevensOnComeOutRoll  = 0;
    unsigned numCurCnsectvSevensOnComeOutRoll  = 0;
    
    unsigned numElevensOnComeOutRoll = 0; // How often 11 appeared on come out.
    unsigned numTwosOnComeOutRoll    = 0; // How often  2 appeared on come out.
    unsigned numThreesOnComeOutRoll  = 0; // How often  3 appeared on come out.
    unsigned numTwelvesOnComeOutRoll = 0; // How often 12 appeared on come out.
    unsigned numCrapsOnComeOutRoll   = 0; // How often craps on come out.

    unsigned numHardwayWinsOn4  = 0;
    unsigned numHardwayWinsOn6  = 0;
    unsigned numHardwayWinsOn8  = 0;
    unsigned numHardwayWinsOn10 = 0;
    
    unsigned numCurCnsectvHardwayWinsOn4  = 0;
    unsigned numCurCnsectvHardwayWinsOn6  = 0;
    unsigned numCurCnsectvHardwayWinsOn8  = 0;
    unsigned numCurCnsectvHardwayWinsOn10 = 0;
    
    unsigned numMaxCnsectvHardwayWinsOn4  = 0;
    unsigned numMaxCnsectvHardwayWinsOn6  = 0;
    unsigned numMaxCnsectvHardwayWinsOn8  = 0;
    unsigned numMaxCnsectvHardwayWinsOn10 = 0;
    
    unsigned numHardwayLoseOn4  = 0;
    unsigned numHardwayLoseOn6  = 0;
    unsigned numHardwayLoseOn8  = 0;
    unsigned numHardwayLoseOn10 = 0;
    
    unsigned numCurCnsectvHardwayLoseOn4  = 0;
    unsigned numCurCnsectvHardwayLoseOn6  = 0;
    unsigned numCurCnsectvHardwayLoseOn8  = 0;
    unsigned numCurCnsectvHardwayLoseOn10 = 0;
    
    unsigned numMaxCnsectvHardwayLoseOn4  = 0;
    unsigned numMaxCnsectvHardwayLoseOn6  = 0;
    unsigned numMaxCnsectvHardwayLoseOn8  = 0;
    unsigned numMaxCnsectvHardwayLoseOn10 = 0;
    
    unsigned numTurnsShooter               = 0; // Same as sevenOut count
    unsigned numCurCnsectvRollsThisShooter = 0; // Count rolls current shooter.
    unsigned numMaxCnsectvRollsThisShooter = 0; // Longest roll ever before 7-out.
    unsigned totCnsectvRollsPerShooter     = 0;
    
private:
    void countDiceNumbers  (const Dice& curRoll, const Dice& prevRoll);
    void countComeOutRolls (unsigned point);
    void countPointRolls   (unsigned point, unsigned roll);
    void countShooterRolls (unsigned roll);
    void countFieldBetWins (unsigned roll);
    void countFieldBetLose (unsigned roll);
    void countHardwayWins  (unsigned point, unsigned d1, unsigned d2);
    void countHardwayLose  (unsigned point, unsigned d1, unsigned d2);
    void countPassLineWins (unsigned roll);
    void countPassLineLose (unsigned roll);
    void countDontPassWins (unsigned roll);
    void countDontPassLose (unsigned roll);
    void countComeWins     (unsigned point, unsigned roll);
    void countComeLose     (unsigned point, unsigned roll);
    void countDontComeWins (unsigned point, unsigned roll);
    void countDontComeLose (unsigned point, unsigned roll);
    
    void bumpPassLineWins();
    void bumpPassLineLose();
    void bumpDontPassWins();
    void bumpDontPassLose();
    void bumpComeWins();
    void bumpComeLose();
    void bumpDontComeWins();
    void bumpDontComeLose();
    void bumpHardwayWins(unsigned roll);
    void bumpHardwayLose(unsigned roll);
    void bumpSevenOuts();
    void bumpSevensOnComeOutRoll();
    
    void update2 (unsigned point);
    void update3 (unsigned point);
    void update7 (unsigned point);
    void update11(unsigned point);
    void update12(unsigned point);
    void updatePointRoll(unsigned point, unsigned roll);
};

/*-----------------------------------------------------------*//**

@class TableStats

@brief Track statistics about rolls and dice

*/

} // namespace Craps

//----------------------------------------------------------------
