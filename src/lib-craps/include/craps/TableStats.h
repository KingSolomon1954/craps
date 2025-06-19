//----------------------------------------------------------------
//
// File: TableStats.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <deque>
#include <map>
#include <string>
#include <craps/Dice.h>
#include <craps/CrapsBetIntfc.h>
#include <controller/Globals.h>
#include <gen/Timepoint.h>

namespace Craps {

class TableStats
{
public:
    /// @name Lifecycle
    /// @{
    explicit TableStats(const std::string& tableId);
    /// @}

    /// @name Modifiers
    /// @{
    void recordWin (CrapsBetIntfc* bet, Gbl::Money amtWin);
    void recordLose(CrapsBetIntfc* bet, Gbl::Money amtLose);
    void recordKeep(CrapsBetIntfc* bet);
    void recordDiceRoll(unsigned point, const Dice& curRoll);
    void reset();
    void setRollHistorySize(size_t rollHistorySize);
    /// @}

    /// @name Observers
    /// @{
    size_t getRollHistorySize() const;
    /// @}

    std::string tableId;
    Gen::Timepoint sessionStart;
    Gen::Timepoint sessionEnd;
    
    std::deque<Dice> recentRolls;  // Front element is oldest roll
    
    struct Counter
    {
        unsigned count_ = 0;
        bool armed = false;
        unsigned curRepeats = 0;
        unsigned maxRepeats = 0;
        unsigned pivot = 0;
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

    // Betting Stats

    struct BetStat
    {
        unsigned count = 0;
        unsigned distance = 0;
    };
    
    struct BetTypesWinLose
    {
        std::map<std::string, BetStat> wins;
        std::map<std::string, BetStat> lose;
    };
    BetTypesWinLose betsWinLose;
    
    unsigned totNumBetsAllBets = 0;
    unsigned totNumWinsAllBets = 0;
    unsigned totNumLoseAllBets = 0;
    unsigned totNumKeepAllBets = 0;

    Gbl::Money totAmtAllBets     = 0;
    Gbl::Money totAmtWinsAllBets = 0;
    Gbl::Money totAmtLoseAllBets = 0;
    Gbl::Money totAmtKeepAllBets = 0;

    Gbl::Money maxAmtBetOneBet  = 0;
    Gbl::Money maxAmtWinOneBet  = 0;
    Gbl::Money maxAmtLoseOneBet = 0;
    Gbl::Money maxAmtKeepOneBet = 0;

    // double winPctg =  (numBetsWin  / numBetsMade) * 100
    // double losePctg = (numBetsLose / numBetsMade) * 100
    // double pushPctg = (numBetsPush / numBetsMade) * 100

    struct NumBets
    {
        unsigned current = 0;
        unsigned max     = 0;
        unsigned total   = 0;
        void reset();
    };
    
    NumBets numBetsOneRoll;
    NumBets numBetsWinOneRoll;
    NumBets numBetsLoseOneRoll;
    NumBets numBetsKeepOneRoll;
    
    struct AmtBets
    {
        Gbl::Money current = 0;
        Gbl::Money max     = 0;
        Gbl::Money total   = 0;
        void reset();
    };

    AmtBets amtBetsOneRoll;
    AmtBets amtBetsWinOneRoll;
    AmtBets amtBetsLoseOneRoll;
    AmtBets amtBetsKeepOneRoll;

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
    size_t rollHistorySize_ = 25;
    
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
    void disarmSomeCounts  (unsigned point, unsigned roll);
    void resetRollCounts();  // Should be private, called only by CrapsTable
    
    void bumpHardwayWins(unsigned roll);
    void bumpHardwayLose(unsigned roll);
    
    void update2 (unsigned point);
    void update3 (unsigned point);
    void update7 (unsigned point);
    void update11(unsigned point);
    void update12(unsigned point);
    void updatePointRoll(unsigned point, unsigned roll);
    void recordCommon(Gbl::Money amtBet);
    void bumpRecentRolls(const Dice& dice);
};

/*-----------------------------------------------------------*//**

@class TableStats

@brief Track statistics about rolls and dice

*/

} // namespace Craps

//----------------------------------------------------------------
