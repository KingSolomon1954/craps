//----------------------------------------------------------------
//
// File: BetStats.h
//
// Contains several small bet stats sub classes.
//
//----------------------------------------------------------------

#pragma once

#include <map>
#include <craps/CrapsBet.h>
#include <gen/MoneyUtils.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

struct BetStat
{
    unsigned count       = 0;
    unsigned totDistance = 0;
    Gen::Money amount    = 0;
    Gen::Money amountBet = 0;
    void reset();
    void merge(const BetStat& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BetStat&) const = default;  // Only generates ==
};

struct BetOutcomeStats
{
    BetStat wins;
    BetStat losses;
    void reset();
    void merge(const BetOutcomeStats& other);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BetOutcomeStats&) const = default;
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
//
// Selects which recorded outcomes BetStats::tally() adds together.
//
//   ComeOut : decisions reached on a come out roll (stored with no pivot)
//   Points  : decisions reached after a point/pivot was established
//             (PassLine4..10, Come4..10, ...)
//   All     : ComeOut + Points
//
enum class BetScope { ComeOut, Points, All };

//----------------------------------------------------------------

class BetStats
{
public:
    BetStats();
    std::map<std::string, BetOutcomeStats> byBet;

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

    void recordWin (const CrapsBet& bet, Gen::Money amtWin,  unsigned roll = 0);
    void recordLose(const CrapsBet& bet, Gen::Money amtLose, unsigned roll = 0);
    void recordKeep(const CrapsBet& bet);
    void recordCommon(Gen::Money amtBet);
    void resetRollCounts();
    std::string expandBetName(const CrapsBet& bet) const;

    // Read-time aggregation of byBet. See BetScope.
    BetOutcomeStats tally(BetName name, BetScope scope = BetScope::All) const;
    BetOutcomeStats tallyPoint(BetName name, unsigned point) const;
    BetOutcomeStats tallyRoll (BetName name, unsigned roll)  const;
    BetOutcomeStats tallyDoSide  (BetScope scope = BetScope::All) const;
    BetOutcomeStats tallyDontSide(BetScope scope = BetScope::All) const;
    void reset();
    void merge(const BetStats& session);
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    bool operator==(const BetStats&) const = default;  // Only generates ==
};

} // namespace Craps

//----------------------------------------------------------------
