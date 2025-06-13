//----------------------------------------------------------------
//
// File: TableStats.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/Dice.h>
#include <controller/Globals.h>
#include <gen/Timepoint.h>

namespace Craps {

class TableStats
{
public:
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
    
    unsigned numRolls = 0;
    unsigned numSevenOut = 0;                   // Number of times dice passed.

    unsigned numComeOutRolls           = 0;
    unsigned numCurCnsectvComeOutRolls = 0;
    unsigned numMaxCnsectvComeOutRolls = 0;
    
    unsigned numNonComeOutRolls           = 0;
    unsigned numCurCnsectvNonComeOutRolls = 0;
    unsigned numMaxCnsectvNonComeOutRolls = 0;
    
    unsigned numPassLineWins = 0;
    unsigned numPassLineLoss = 0;
    unsigned numDontPassWins = 0;
    unsigned numDontPassLoss = 0;
    unsigned numFieldBetWins = 0;
    unsigned numFieldBetLoss = 0;
    
    unsigned numCurCnsectvPassLineWins = 0;
    unsigned numCurCnsectvPassLineLoss = 0;
    unsigned numCurCnsectvDontPassWins = 0;
    unsigned numCurCnsectvDontPassLoss = 0;
    unsigned numCurCnsectvFieldBetWins = 0;
    unsigned numCurCnsectvFieldBetLoss = 0;
    
    unsigned numMaxCnsectvPassLineWins = 0;
    unsigned numMaxCnsectvPassLineLoss = 0;
    unsigned numMaxCnsectvDontPassWins = 0;
    unsigned numMaxCnsectvDontPassLoss = 0;
    unsigned numMaxCnsectvFieldBetWins = 0;
    unsigned numMaxCnsectvFieldBetLoss = 0;
    
    unsigned numSevensOnComeOutRoll  = 0; // How often  7 appeared on come out.
    unsigned numElevensOnComeOutRoll = 0; // How often 11 appeared on come out.
    unsigned numTwosOnComeOutRoll    = 0; // How often  2 appeared on come out.
    unsigned numThreesOnComeOutRoll  = 0; // How often  3 appeared on come out.
    unsigned numTwelvesOnComeOutRoll = 0; // How often 12 appeared on come out.
    unsigned numCrapsOnComeOutRoll   = 0; // How often craps on come out.
    
    unsigned numCurCnsectvSevensOnComeOutRoll  = 0;
    unsigned numCurCnsectvElevensOnComeOutRoll = 0;
    unsigned numCurCnsectvTwosOnComeOutRoll    = 0;
    unsigned numCurCnsectvThreesOnComeOutRoll  = 0;
    unsigned numCurCnsectvTwelvesOnComeOutRoll = 0;
    unsigned numCurCnsectvCrapsOnComeOutRoll   = 0;
    
    unsigned numMaxCnsectvSevensOnComeOutRoll  = 0;
    unsigned numMaxCnsectvElevensOnComeOutRoll = 0;
    unsigned numMaxCnsectvTwosOnComeOutRoll    = 0;
    unsigned numMaxCnsectvThreesOnComeOutRoll  = 0;
    unsigned numMaxCnsectvTwelvesOnComeOutRoll = 0;
    unsigned numMaxCnsectvCrapsOnComeOutRoll   = 0;
    
    unsigned numComeWinsOn4  = 0;
    unsigned numComeWinsOn5  = 0;
    unsigned numComeWinsOn6  = 0;
    unsigned numComeWinsOn8  = 0;
    unsigned numComeWinsOn9  = 0;
    unsigned numComeWinsOn10 = 0;
    
    unsigned numMaxCnsectvComeWinsOn4  = 0;
    unsigned numMaxCnsectvComeWinsOn5  = 0;
    unsigned numMaxCnsectvComeWinsOn6  = 0;
    unsigned numMaxCnsectvComeWinsOn8  = 0;
    unsigned numMaxCnsectvComeWinsOn9  = 0;
    unsigned numMaxCnsectvComeWinsOn10 = 0;
    
    unsigned numCurCnsectvComeWinsOn4  = 0;
    unsigned numCurCnsectvComeWinsOn5  = 0;
    unsigned numCurCnsectvComeWinsOn6  = 0;
    unsigned numCurCnsectvComeWinsOn8  = 0;
    unsigned numCurCnsectvComeWinsOn9  = 0;
    unsigned numCurCnsectvComeWinsOn10 = 0;
    
    unsigned numComeLossOn4  = 0;
    unsigned numComeLossOn5  = 0;
    unsigned numComeLossOn6  = 0;
    unsigned numComeLossOn8  = 0;
    unsigned numComeLossOn9  = 0;
    unsigned numComeLossOn10 = 0;
    
    unsigned numMaxCnsectvComeLossOn4  = 0;
    unsigned numMaxCnsectvComeLossOn5  = 0;
    unsigned numMaxCnsectvComeLossOn6  = 0;
    unsigned numMaxCnsectvComeLossOn8  = 0;
    unsigned numMaxCnsectvComeLossOn9  = 0;
    unsigned numMaxCnsectvComeLossOn10 = 0;
    
    unsigned numCurCnsectvComeLossOn4  = 0;
    unsigned numCurCnsectvComeLossOn5  = 0;
    unsigned numCurCnsectvComeLossOn6  = 0;
    unsigned numCurCnsectvComeLossOn8  = 0;
    unsigned numCurCnsectvComeLossOn9  = 0;
    unsigned numCurCnsectvComeLossOn10 = 0;
    
    unsigned numDontWinsOn4  = 0;
    unsigned numDontWinsOn5  = 0;
    unsigned numDontWinsOn6  = 0;
    unsigned numDontWinsOn8  = 0;
    unsigned numDontWinsOn9  = 0;
    unsigned numDontWinsOn10 = 0;
    
    unsigned numMaxCnsectvDontWinsOn4  = 0;
    unsigned numMaxCnsectvDontWinsOn5  = 0;
    unsigned numMaxCnsectvDontWinsOn6  = 0;
    unsigned numMaxCnsectvDontWinsOn8  = 0;
    unsigned numMaxCnsectvDontWinsOn9  = 0;
    unsigned numMaxCnsectvDontWinsOn10 = 0;
    
    unsigned numCurCnsectvDontWinsOn4  = 0;
    unsigned numCurCnsectvDontWinsOn5  = 0;
    unsigned numCurCnsectvDontWinsOn6  = 0;
    unsigned numCurCnsectvDontWinsOn8  = 0;
    unsigned numCurCnsectvDontWinsOn9  = 0;
    unsigned numCurCnsectvDontWinsOn10 = 0;
    
    unsigned numDontLossOn4  = 0;
    unsigned numDontLossOn5  = 0;
    unsigned numDontLossOn6  = 0;
    unsigned numDontLossOn8  = 0;
    unsigned numDontLossOn9  = 0;
    unsigned numDontLossOn10 = 0;
   
    unsigned numMaxCnsectvDontLossOn4  = 0;
    unsigned numMaxCnsectvDontLossOn5  = 0;
    unsigned numMaxCnsectvDontLossOn6  = 0;
    unsigned numMaxCnsectvDontLossOn8  = 0;
    unsigned numMaxCnsectvDontLossOn9  = 0;
    unsigned numMaxCnsectvDontLossOn10 = 0;
    
    unsigned numCurCnsectvDontLossOn4  = 0;
    unsigned numCurCnsectvDontLossOn5  = 0;
    unsigned numCurCnsectvDontLossOn6  = 0;
    unsigned numCurCnsectvDontLossOn8  = 0;
    unsigned numCurCnsectvDontLossOn9  = 0;
    unsigned numCurCnsectvDontLossOn10 = 0;
    
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
    
    unsigned numHardwayLossOn4  = 0;
    unsigned numHardwayLossOn6  = 0;
    unsigned numHardwayLossOn8  = 0;
    unsigned numHardwayLossOn10 = 0;
    
    unsigned numCurCnsectvHardwayLossOn4  = 0;
    unsigned numCurCnsectvHardwayLossOn6  = 0;
    unsigned numCurCnsectvHardwayLossOn8  = 0;
    unsigned numCurCnsectvHardwayLossOn10 = 0;
    
    unsigned numMaxCnsectvHardwayLossOn4  = 0;
    unsigned numMaxCnsectvHardwayLossOn6  = 0;
    unsigned numMaxCnsectvHardwayLossOn8  = 0;
    unsigned numMaxCnsectvHardwayLossOn10 = 0;
    
    unsigned num2  = 0;
    unsigned num3  = 0;
    unsigned num4  = 0;
    unsigned num5  = 0;
    unsigned num6  = 0;
    unsigned num7  = 0;
    unsigned num8  = 0;
    unsigned num9  = 0;
    unsigned num10 = 0;
    unsigned num11 = 0;
    unsigned num12 = 0;
    
    unsigned numCurCnsectvNum2  = 0;
    unsigned numCurCnsectvNum3  = 0;
    unsigned numCurCnsectvNum4  = 0;
    unsigned numCurCnsectvNum5  = 0;
    unsigned numCurCnsectvNum6  = 0;
    unsigned numCurCnsectvNum7  = 0;
    unsigned numCurCnsectvNum8  = 0;
    unsigned numCurCnsectvNum9  = 0;
    unsigned numCurCnsectvNum10 = 0;
    unsigned numCurCnsectvNum11 = 0;
    unsigned numCurCnsectvNum12 = 0;

    unsigned numMaxCnsectvNum2  = 0;
    unsigned numMaxCnsectvNum3  = 0;
    unsigned numMaxCnsectvNum4  = 0;
    unsigned numMaxCnsectvNum5  = 0;
    unsigned numMaxCnsectvNum6  = 0;
    unsigned numMaxCnsectvNum7  = 0;
    unsigned numMaxCnsectvNum8  = 0;
    unsigned numMaxCnsectvNum9  = 0;
    unsigned numMaxCnsectvNum10 = 0;
    unsigned numMaxCnsectvNum11 = 0;
    unsigned numMaxCnsectvNum12 = 0;

    unsigned numTurnsShooter               = 0; // Same as sevenOut count
    unsigned numCurCnsectvRollsThisShooter = 0; // Count rolls current shooter.
    unsigned numMaxCnsectvRollsThisShooter = 0; // Longest roll ever before 7-out.
    unsigned totCnsectvRollsPerShooter     = 0;
    
private:
    void incrementComeOutRolls();
    void incrementNonComeOutRolls();
    void incrementPassLineWins();
    void incrementPassLineLoss();
    void incrementDontPassWins();
    void incrementDontPassLoss();
    void incrementFieldBetWins();
    void incrementFieldBetLoss();
    void incrementHardwayWinsOn4();
    void incrementHardwayLossOn4();
    void incrementHardwayWinsOn6();
    void incrementHardwayLossOn6();
    void incrementHardwayWinsOn8();
    void incrementHardwayLossOn8();
    void incrementHardwayWinsOn10();
    void incrementHardwayLossOn10();
    void incrementComeWinsOn4();
    void incrementComeWinsOn5();
    void incrementComeWinsOn6();
    void incrementComeWinsOn8();
    void incrementComeWinsOn9();
    void incrementComeWinsOn10();
    void incrementComeLossOn4();
    void incrementComeLossOn5();
    void incrementComeLossOn6();
    void incrementComeLossOn8();
    void incrementComeLossOn9();
    void incrementComeLossOn10();
    void incrementDontWinsOn4();
    void incrementDontWinsOn5();
    void incrementDontWinsOn6();
    void incrementDontWinsOn8();
    void incrementDontWinsOn9();
    void incrementDontWinsOn10();
    void incrementDontLossOn4();
    void incrementDontLossOn5();
    void incrementDontLossOn6();
    void incrementDontLossOn8();
    void incrementDontLossOn9();
    void incrementDontLossOn10();
    void incrementCrapsOnComeOutRoll();
    void incrementTwosOnComeOutRoll();
    void incrementThreesOnComeOutRoll();
    void incrementSevensOnComeOutRoll();
    void incrementElevensOnComeOutRoll();
    void incrementTwelvesOnComeOutRoll();
    void incrementRollsThisShooter();
    void incrementSevenOut();
    void resetComeOutRollCounters();
    void update2(unsigned point);
    void update3(unsigned point);
    void update4(unsigned d1, unsigned d2);
    void update5();
    void update6(unsigned d1, unsigned d2);
    void update7(unsigned point);
    void update8(unsigned d1, unsigned d2);
    void update9();
    void update10(unsigned d1, unsigned d2);
    void update11(unsigned point);
    void update12(unsigned point);
    
    Dice prevRoll_;
    bool armed4  = false;
    bool armed5  = false;
    bool armed6  = false;
    bool armed7  = false;
    bool armed8  = false;
    bool armed9  = false;
    bool armed10 = false;
};

/*-----------------------------------------------------------*//**

@class TableStats

@brief Track statistics about rolls and dice

*/

} // namespace Craps

//----------------------------------------------------------------
