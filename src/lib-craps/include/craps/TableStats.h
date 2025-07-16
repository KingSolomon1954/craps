//----------------------------------------------------------------
//
// File: TableStats.h
//
//----------------------------------------------------------------

#pragma once

#include <deque>
#include <string>
#include <craps/BetStats.h>
#include <craps/CrapsBetIntfc.h>
#include <craps/Dice.h>
#include <craps/BankStats.h>
#include <craps/RollStats.h>
#include <craps/SessionHistory.h>
#include <gen/MoneyUtil.h>
#include <gen/Timepoint.h>
#include <yaml-cpp/yaml.h>

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
    void recordWin       (const CrapsBetIntfc& bet, Gen::Money amtWin);
    void recordLose      (const CrapsBetIntfc& bet, Gen::Money amtLose);
    void recordKeep      (const CrapsBetIntfc& bet);
    void recordDiceRoll  (unsigned point, const Dice& curRoll);
    void recordWithdrawal(Gen::Money amount);
    void recordDeposit   (Gen::Money amount);
    void recordRefill    (Gen::Money amount);
    void setRollHistorySize(size_t rollHistorySize);
    void merge(const TableStats& session);
    void saveFile(const std::string& dir) const;
    void loadFile(const std::string& dir);
    void reset();
    /// @}

    /// @name Observers
    /// @{
    size_t getRollHistorySize() const;
    bool operator==(const TableStats&) const = default;  // Only generates ==
    /// @}

    // All these are saved and read from TableStats file.
    std::string      tableId;
    BetStats         betStats;
    RollStats        rollStats;
    BankStats        moneyStats;
    SessionHistory   sessionHistory;

    // These not saved to TableStats file.
    std::deque<Dice> recentRolls;  // Roll history. Front element is oldest roll
    Gen::Timepoint   sessionStartDate;

private:
    size_t rollHistorySize_ = 25;

    void countDiceNumbers     (unsigned roll);
    void countAllPoints       (unsigned roll);
    void countComeOutRolls    (unsigned point);
    void countPointRolls      (unsigned point, unsigned roll);
    void countShooterRolls    (unsigned point, unsigned roll);
    void countFieldBetWins    (unsigned roll);
    void countFieldBetLose    (unsigned roll);
    void countHardwayWins     (unsigned point, unsigned d1, unsigned d2);
    void countHardwayLose     (unsigned point, unsigned d1, unsigned d2);
    void countPassLinePntWins (unsigned roll);
    void countPassLinePntLose (unsigned roll);
    void countDontPassPntWins (unsigned roll);
    void countDontPassPntLose (unsigned roll);
    void countComePntWins     (unsigned point, unsigned roll);
    void countComePntLose     (unsigned point, unsigned roll);
    void countDontComePntWins (unsigned point, unsigned roll);
    void countDontComePntLose (unsigned point, unsigned roll);
    void disarmSomeCounts     (unsigned point, unsigned roll);
    void bumpHardwayWins      (unsigned roll);
    void bumpHardwayLose      (unsigned roll);
    void bumpRecentRolls      (const Dice& dice);
    void resetRollCounts();

    void update2 (unsigned point);
    void update3 (unsigned point);
    void update7 (unsigned point);
    void update11(unsigned point);
    void update12(unsigned point);
    void updatePointRoll(unsigned point, unsigned roll);
    void recordCommon(Gen::Money amtBet);

    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
};

/*-----------------------------------------------------------*//**

@class TableStats

@brief Track statistics about rolls and dice

*/

} // namespace Craps

//----------------------------------------------------------------
