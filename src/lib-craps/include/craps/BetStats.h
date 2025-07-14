//----------------------------------------------------------------
//
// File: BetStats.h
//
// Contains several small bet stats sub classes.
//
//----------------------------------------------------------------

#pragma once

#include <map>
#include <gen/MoneyType.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

struct BetStat
{
    unsigned count = 0;
    unsigned totDistance = 0;
    Gen::Money amount;
    Gen::Money amountBet;
    void reset();
    void merge(const BetStat& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BetStat&) const = default;  // Only generates ==
};

struct BetTypeStats
{
    BetTypeStats();
    std::map<std::string, BetStat> wins;
    std::map<std::string, BetStat> lose;
    void reset();
    void merge(const BetTypeStats& bts);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BetTypeStats&) const = default;  // Only generates ==
};

struct NumBets
{
    unsigned current = 0;
    unsigned max     = 0;
    unsigned total   = 0;
    void reset();
    void merge(const NumBets& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const NumBets&) const = default;  // Only generates ==
};

struct AmtBets
{
    Gen::Money current = 0;
    Gen::Money max     = 0;
    Gen::Money total   = 0;
    void reset();
    void merge(const AmtBets& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const AmtBets&) const = default;  // Only generates ==
};

//----------------------------------------------------------------

class CrapsBetIntfc; // fwd

class BetStats
{
public:
    BetTypeStats betTypeStats;

    unsigned totNumBetsAllBets = 0;
    unsigned totNumWinsAllBets = 0;
    unsigned totNumLoseAllBets = 0;
    unsigned totNumKeepAllBets = 0;

    Gen::Money totAmtAllBets     = 0;
    Gen::Money totAmtWinsAllBets = 0;
    Gen::Money totAmtLoseAllBets = 0;
    Gen::Money totAmtKeepAllBets = 0;

    Gen::Money maxAmtBetOneBet  = 0;
    Gen::Money maxAmtWinOneBet  = 0;
    Gen::Money maxAmtLoseOneBet = 0;
    Gen::Money maxAmtKeepOneBet = 0;

    NumBets numBetsOneRoll;
    NumBets numBetsWinOneRoll;
    NumBets numBetsLoseOneRoll;
    NumBets numBetsKeepOneRoll;

    AmtBets amtBetsOneRoll;
    AmtBets amtBetsWinOneRoll;
    AmtBets amtBetsLoseOneRoll;
    AmtBets amtBetsKeepOneRoll;

    std::string expandBetName(const CrapsBetIntfc& bet) const;
    void reset();
    void merge(const BetStats& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BetStats&) const = default;  // Only generates ==
};

} // namespace Craps

//----------------------------------------------------------------
