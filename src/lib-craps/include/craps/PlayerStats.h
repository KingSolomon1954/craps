//----------------------------------------------------------------
//
// File: PlayerStats.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <craps/BetStats.h>
#include <craps/CrapsBet.h>
#include <craps/Repeaters.h>
#include <craps/SessionHistory.h>
#include <gen/MoneyUtils.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

class PlayerStats
{
public:
    /// @name Lifecycle
    /// @{
    PlayerStats() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void recordWin (const CrapsBet& bet, Gen::Money amtWin, unsigned roll = 0);
    void recordLose(const CrapsBet& bet, Gen::Money amtLose, unsigned roll = 0);
    void recordWin (const DecisionRecord& dr);
    void recordLose(const DecisionRecord& dr);
    void recordKeep(const CrapsBet& bet);
    void merge     (const PlayerStats& session);
    void reset();
    /// @}

    /// @name Observers
    /// @{
    bool operator==(const PlayerStats&) const = default;  // Only generates ==
    /// @}

    // YAML operations
    void toYAML(YAML::Node& node) const;
    void fromYAML(const YAML::Node& node);

    // These are saved and read from YAML file.
    BetStats       betStats;
    SessionHistory sessionHistory;

    // Hits of a point number before a 7, counting only what this player
    // took part in (see Repeaters and Player::recordRepeaters()).
    Repeaters pointRepeaters{PointNums};  // Pass line and Come, any 7 ends
    Repeaters placeRepeaters{PointNums};  // Place bets, seven-out ends
    Repeaters buyRepeaters  {PointNums};  // Buy bets, seven-out ends

private:
};

/*-----------------------------------------------------------*//**

@class PlayerStats

@brief Track statistics about rolls and dice

*/

} // namespace Craps

//----------------------------------------------------------------
