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
    struct Counter
    {
        unsigned count_ = 0;
        bool armed = false;
        unsigned curRepeats = 0;
        unsigned maxRepeats = 0;
        unsigned count() const;
        unsigned repeats() const;
        void bump();
        void disarm();
        void reset();
    };
    
    struct PointCounts
    {
        Counter wins;
        Counter lose;
        void reset();
    };
    
    std::array<Counter,     13> numberCounts   {};  // Index 2-12 used
    std::array<PointCounts, 11> passLineCounts {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> dontPassCounts {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> comeCounts     {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> dontComeCounts {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> hardwayCounts  {};  // Index 4,6,8,10 used

    /// @name Lifecycle
    /// @{
    explicit TableStats(const std::string& tableId);
    /// @}

    /// @name Modifiers
    /// @{
    void updateAddBet(Gbl::Money betAmount);
    void updateOddsBet(Gbl::Money contractAmount, Gbl::Money oddsAmount);
    void updateBetsAfterThrow(Gbl::Money amtOnTable,
                              const std::pair<unsigned, Gbl::Money>& winStats,
                              const std::pair<unsigned, Gbl::Money>& loseStats,
                              unsigned numKeeps);
    void updateDiceRoll(unsigned point, const Dice& curRoll);
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
    unsigned numRolls = 0;
    Counter comeOutRolls;
    Counter pointRolls;
    Counter passLineWins;
    Counter passLineLose;
    Counter dontPassWins;
    Counter dontPassLose;
    Counter comeWins;
    Counter comeLose;
    Counter dontComeWins;
    Counter dontComeLose;
    Counter fieldBetWins;
    Counter fieldBetLose;
    Counter sevenOuts;
    Counter shooterCounts;
    Counter twosOnComeOutRoll;
    Counter threesOnComeOutRoll;
    Counter sevensOnComeOutRoll;
    Counter elevensOnComeOutRoll;
    Counter twelvesOnComeOutRoll;
    Counter crapsOnComeOutRoll;
    
private:
    void countDiceNumbers  (unsigned roll);
    void countComeOutRolls (unsigned point);
    void countPointRolls   (unsigned point, unsigned roll);
    void countShooterRolls (unsigned point, unsigned roll);
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
    void disarmThese       (unsigned point, unsigned roll);
    
    void bumpHardwayWins(unsigned roll);
    void bumpHardwayLose(unsigned roll);
    
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
