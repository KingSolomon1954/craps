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
#include <craps/DecisionRecord.h>
#include <gen/MoneyUtils.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

struct BetStat
{
    unsigned count       = 0;
    unsigned totDistance = 0;
    Gen::Money amount    = 0;   // Won or lost, odds included
    Gen::Money amountBet = 0;   // Wagered, odds included
    Gen::Money oddsBet    = 0;  // Part of amountBet that was odds
    Gen::Money oddsAmount = 0;  // Part of amount that was won/lost on odds
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
//
// How a roll dependent bet fared on a particular number.
//
//   EntireLoss : the whole wager lost (e.g. World on a point number)
//   PartialWin : net win, but some components lost (Horn on 2, C&E on 11)
//   BreakEven  : a component won but net profit is zero (World on 7)
//   CleanWin   : wins with no losing components (Field, AnySeven, ...)
//
enum class RollOutcome { EntireLoss, PartialWin, BreakEven, CleanWin };

// Hardway losses split by what ended the bet.
struct HardwayLosses
{
    BetStat easy;    // Rolled the number the easy way
    BetStat seven;   // Rolled a seven
};

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
    void recordWin (const DecisionRecord& dr);
    void recordLose(const DecisionRecord& dr);
    void recordKeep(const CrapsBet& bet);
    void recordCommon(Gen::Money amtBet);
    void recordOutcome(bool win, const CrapsBet& bet, Gen::Money amount,
                       unsigned roll, Gen::Money oddsAmt);
    void resetRollCounts();
    std::string expandBetName(const CrapsBet& bet) const;

    // Read-time aggregation of byBet. See BetScope.
    BetOutcomeStats tally(BetName name, BetScope scope = BetScope::All) const;
    BetOutcomeStats tallyPoint(BetName name, unsigned point) const;
    BetOutcomeStats tallyRoll (BetName name, unsigned roll)  const;
    HardwayLosses   tallyHardwayLosses(unsigned point) const;
    BetOutcomeStats tallyRollOutcome(BetName name, RollOutcome kind) const;
    static RollOutcome classifyRoll(BetName name, unsigned roll);
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
