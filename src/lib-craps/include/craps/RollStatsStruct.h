//----------------------------------------------------------------
//
// File: RollStatsStruct.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <map>
#include <yaml-cpp/yaml.h>

namespace Craps {

struct Counter
{
    unsigned count_     = 0;
    unsigned curRepeats = 0;
    unsigned maxRepeats = 0;
    unsigned pivot      = 0;
    bool armed          = false;

    unsigned count()   const;
    unsigned repeats() const;
    void bump();
    void disarm();
    void reset();
    void merge(const Counter& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const Counter&) const = default;  // Only generates ==
};

struct PointCounts
{
    Counter wins;
    Counter lose;
    void reset();
    void merge(const PointCounts& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const PointCounts&) const = default;  // Only generates ==
};

//----------------------------------------------------------------

class RollStats
{
public:
    std::array<Counter,     13> numberCounts   {};  // Index 2-12 used
    std::array<Counter,     11> anyEstPntCnts  {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> passPntCnts    {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> comePntCnts    {};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> dontPassPntCnts{};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> dontComePntCnts{};  // Index 4,5,6,8,9,10 used
    std::array<PointCounts, 11> hardwayCounts  {};  // Index 4,6,8,10 used

    unsigned numRolls             = 0;
    unsigned numPointsEstablished = 0;
    Counter comeOutRolls;
    Counter pointRolls;
    Counter passWinsComeOut;
    Counter passLoseComeOut;
    Counter comeWinsComeOut;
    Counter comeLoseComeOut;
    Counter dontPassWinsComeOut;
    Counter dontPassLoseComeOut;
    Counter dontComeWinsComeOut;
    Counter dontComeLoseComeOut;
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
    void reset();
    void merge(const RollStats& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const RollStats&) const = default;  // Only generates ==

private:    
    void numberCountsToYAML(YAML::Node& node) const;
    void pointCountsToYAML (YAML::Node& node) const;
    void singleCountsToYAML(YAML::Node& node) const;
    void numberCountsFromYAML(const YAML::Node& node);
    void pointCountsFromYAML (const YAML::Node& node);
    void singleCountsFromYAML(const YAML::Node& node);
};
    
} // namespace Craps

//----------------------------------------------------------------
