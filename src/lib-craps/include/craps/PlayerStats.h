//----------------------------------------------------------------
//
// File: PlayerStats.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <craps/BetStats.h>
#include <craps/CrapsBet.h>
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
    void recordWin (const CrapsBet& bet, Gen::Money amtWin);
    void recordLose(const CrapsBet& bet, Gen::Money amtLose);
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

private:
};

/*-----------------------------------------------------------*//**

@class PlayerStats

@brief Track statistics about rolls and dice

*/

} // namespace Craps

//----------------------------------------------------------------
