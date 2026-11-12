//----------------------------------------------------------------
//
// File: BetStats.cpp
//
//----------------------------------------------------------------

#include <craps/BetStats.h>

using namespace Craps;

namespace
{
std::string
statsName(BetName name)
{
    std::string s = EnumBetName::toString(name);
    return s == "CandE" ? "C&E" : s;
}

bool
isRollBet(BetName name)
{
    return name == BetName::Field    || name == BetName::AnyCraps ||
           name == BetName::AnySeven || name == BetName::CandE    ||
           name == BetName::Horn     || name == BetName::World;
}

constexpr unsigned PointNumbers[] = {4, 5, 6, 8, 9, 10};

// Entry name for a roll dependent bet when a given number was rolled.
//
// Field already records per number: CrapsBet::evalField sets the pivot to
// the number rolled, so expandBetName() yields Field2 .. Field12.
// The other one roll bets keep a pivot of 0 and an overall entry
// (e.g. "Horn"), so they get an additional entry named Horn@11.
// The '@' keeps these from looking like point entries (Horn4, Horn10).
std::string
rollEntryName(BetName name, unsigned roll)
{
    return statsName(name) + (name == BetName::Field ? "" : "@") +
           std::to_string(roll);
}

// Extra per number entry to record, or empty when none is needed.
std::string
rollBetName(const CrapsBet& bet, unsigned roll)
{
    if (roll < 2 || roll > 12 || !isRollBet(bet.betName()) ||
        bet.betName() == BetName::Field)
        return {};
    return rollEntryName(bet.betName(), roll);
}
} // namespace

/*-----------------------------------------------------------*//**

Record a winning bet.

*/
void
BetStats::recordWin(const CrapsBet& bet, Gen::Money amtWin, unsigned roll)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    std::string betName = expandBetName(bet);
    byBet[betName].wins.count++;
    byBet[betName].wins.amountBet   += amtBet;
    byBet[betName].wins.amount      += amtWin;
    byBet[betName].wins.totDistance += bet.distance();
    if (const std::string rollKey = rollBetName(bet, roll); !rollKey.empty())
    {
        auto& w = byBet[rollKey].wins;
        w.count++;
        w.amountBet   += amtBet;
        w.amount      += amtWin;
        w.totDistance += bet.distance();
    }

    totNumBetsAllBets++;
    totAmtAllBets += amtBet;

    totNumWinsAllBets++;
    numBetsWinOneRoll.total++;

    totAmtWinsAllBets += amtWin;
    amtBetsWinOneRoll.total += amtWin;

    numBetsWinOneRoll.current++;

    amtBetsWinOneRoll.current += amtWin;

    maxAmtWinOneBet       = std::max(amtWin, maxAmtWinOneBet);
    amtBetsWinOneRoll.max = std::max(amtBetsWinOneRoll.current, amtBetsWinOneRoll.max);
    numBetsWinOneRoll.max = std::max(numBetsWinOneRoll.current, numBetsWinOneRoll.max);
}

/*-----------------------------------------------------------*//**

Record a losing bet.

*/
void
BetStats::recordLose(const CrapsBet& bet, Gen::Money amtLose, unsigned roll)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    std::string betName = expandBetName(bet);
    byBet[betName].losses.count++;
    byBet[betName].losses.amountBet   += amtBet;
    byBet[betName].losses.amount      += amtLose;
    byBet[betName].losses.totDistance += bet.distance();
    if (const std::string rollKey = rollBetName(bet, roll); !rollKey.empty())
    {
        auto& l = byBet[rollKey].losses;
        l.count++;
        l.amountBet   += amtBet;
        l.amount      += amtLose;
        l.totDistance += bet.distance();
    }

    totNumBetsAllBets++;
    totAmtAllBets += amtBet;

    totNumLoseAllBets++;
    numBetsLoseOneRoll.total++;

    totAmtLoseAllBets += amtLose;
    amtBetsLoseOneRoll.total += amtLose;

    numBetsLoseOneRoll.current++;

    amtBetsLoseOneRoll.current += amtLose;

    maxAmtLoseOneBet       = std::max(amtLose, maxAmtLoseOneBet);
    amtBetsLoseOneRoll.max = std::max(amtBetsLoseOneRoll.current, amtBetsLoseOneRoll.max);
    numBetsLoseOneRoll.max = std::max(numBetsLoseOneRoll.current, numBetsLoseOneRoll.max);
}

/*-----------------------------------------------------------*//**

Record a keeping bet.

No need to count keeps. They will eventually win or lose.
But might want to track avg number of keeps per roll.

*/
void
BetStats::recordKeep(const CrapsBet& bet)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    // totNumBetsAllBets++;      // Don't incr here, counted when win/lose
    // totAmtAllBets += amtBet;  // Don't incr here, counted when win/lose
    totNumKeepAllBets++;
    numBetsKeepOneRoll.total++;

    totAmtKeepAllBets += amtBet;
    amtBetsKeepOneRoll.total += amtBet;

    numBetsKeepOneRoll.current++;

    amtBetsKeepOneRoll.current += amtBet;

    maxAmtKeepOneBet       = std::max(amtBet, maxAmtKeepOneBet);
    amtBetsKeepOneRoll.max = std::max(amtBetsKeepOneRoll.current, amtBetsKeepOneRoll.max);
    numBetsKeepOneRoll.max = std::max(numBetsKeepOneRoll.current, numBetsKeepOneRoll.max);
}

//-----------------------------------------------------------------
//
// Helper function to update common stats between recordWin/Lose/Keep
//
void
BetStats::recordCommon(Gen::Money amtBet)
{
    numBetsOneRoll.total++;
    numBetsOneRoll.current++;
    amtBetsOneRoll.current += amtBet;
    maxAmtBetOneBet    = std::max(amtBet, maxAmtBetOneBet);
    amtBetsOneRoll.max = std::max(amtBetsOneRoll.current, amtBetsOneRoll.max);
    numBetsOneRoll.max = std::max(numBetsOneRoll.current, numBetsOneRoll.max);
}

//----------------------------------------------------------------
//
// Clear some counters from previous roll
//
// Called by TableStats when recording the current dice throw.
//
void
BetStats::resetRollCounts()
{
    numBetsOneRoll.current     = 0;
    numBetsWinOneRoll.current  = 0;
    numBetsLoseOneRoll.current = 0;
    numBetsKeepOneRoll.current = 0;
    amtBetsOneRoll.current     = 0;
    amtBetsWinOneRoll.current  = 0;
    amtBetsLoseOneRoll.current = 0;
    amtBetsKeepOneRoll.current = 0;
}

//-----------------------------------------------------------------

void
BetStat::reset()
{
    count       = 0;
    totDistance = 0;
    amount      = 0;
    amountBet   = 0;
}

//-----------------------------------------------------------------

void
BetStat::merge(const BetStat& session)
{
    count       += session.count;
    totDistance += session.totDistance;
    amount      += session.amount;
    amountBet   += session.amountBet;
}

//-----------------------------------------------------------------

YAML::Node
BetStat::toYAML() const
{
    YAML::Node node;
    node["count"]       = count;
    node["totDistance"] = totDistance;
    node["amount"]      = amount;
    node["amountBet"]   = amountBet;
    return node;
}

//-----------------------------------------------------------------

void
BetStat::fromYAML(const YAML::Node& node)
{
    count       = node["count"].as<unsigned>();
    totDistance = node["totDistance"].as<unsigned>();
    amount      = node["amount"].as<Gen::Money>();
    amountBet   = node["amountBet"].as<Gen::Money>();
}

//-----------------------------------------------------------------

namespace
{
void
initializeBetMap(std::map<std::string, BetOutcomeStats>& byBet)
{
    for (const auto& name : {
        "PassLine", "Come", "DontPass", "DontCome",
        "Place", "Lay", "Buy",
        "Hardway4", "Hardway6", "Hardway8", "Hardway10",
        "Field", "AnyCraps", "AnySeven", "C&E", "Horn", "World"
    })
        byBet.try_emplace(name);

    for (const auto& base : {
        "PassLine", "Come", "DontPass", "DontCome",
        "Place", "Lay", "Buy"
    })
        for (const unsigned point : {4, 5, 6, 8, 9, 10})
            byBet.try_emplace(std::string(base) + std::to_string(point));

    for (unsigned number = 2; number <= 12; ++number)
        byBet.try_emplace("Field" + std::to_string(number));
}
} // namespace

//-----------------------------------------------------------------

BetStats::BetStats()
{
    initializeBetMap(byBet);
}

//-----------------------------------------------------------------

void
BetOutcomeStats::reset()
{
    wins.reset();
    losses.reset();
}

//-----------------------------------------------------------------

void
BetOutcomeStats::merge(const BetOutcomeStats& other)
{
    wins.merge(other.wins);
    losses.merge(other.losses);
}

//-----------------------------------------------------------------

YAML::Node
BetOutcomeStats::toYAML() const
{
    YAML::Node node;
    node["wins"] = wins.toYAML();
    node["losses"] = losses.toYAML();
    return node;
}

//-----------------------------------------------------------------

void
BetOutcomeStats::fromYAML(const YAML::Node& node)
{
    wins.fromYAML(node["wins"]);
    losses.fromYAML(node["losses"]);
}

//-----------------------------------------------------------------

void
NumBets::reset()
{
    current = 0;
    max     = 0;
    total   = 0;
}

//-----------------------------------------------------------------

void
NumBets::merge(const NumBets& session)
{
    max   = std::max(max, session.max);
    total += session.total;
}

//-----------------------------------------------------------------

YAML::Node
NumBets::toYAML() const
{
    YAML::Node node;
    node["total"] = total;
    node["max"]   = max;
    return node;
}

//-----------------------------------------------------------------

void
NumBets::fromYAML(const YAML::Node& node)
{
    max   = node["max"].as<unsigned>();
    total = node["total"].as<unsigned>();
    current = 0;
}

//-----------------------------------------------------------------

void
AmtBets::reset()
{
    current = 0;
    max     = 0;
    total   = 0;
}

//-----------------------------------------------------------------

void
AmtBets::merge(const AmtBets& session)
{
    max   = std::max(max, session.max);
    total += session.total;
}

//-----------------------------------------------------------------

YAML::Node
AmtBets::toYAML() const
{
    YAML::Node node;
    node["total"] = total;
    node["max"]   = max;
    return node;
}

//-----------------------------------------------------------------

void
AmtBets::fromYAML(const YAML::Node& node)
{
    max     = node["max"].as<Gen::Money>();
    total   = node["total"].as<Gen::Money>();
    current = 0;
}

//----------------------------------------------------------------
//
// Helper function to expand bet names with their pivot.
//
// Returns a bet name expanded with its pivot, if any, like this:
//
//   PassLine --> PassLine6
//   Come     --> Come8
//   Hardway  --> Hardway4
//
std::string
BetStats::expandBetName(const CrapsBet& bet) const
{
    std::string betName = statsName(bet.betName());
    if (bet.pivot() != 0)
    {
        betName += std::to_string(bet.pivot());
    }
    return betName;
}

//-----------------------------------------------------------------
//
// Sum recorded outcomes for one bet name.
//
//   ComeOut -> the entry with no pivot ("PassLine")
//   Points  -> entries with a pivot ("PassLine4" ... "PassLine10")
//   All     -> both
//
// Roll dependent bets (Field, Horn, ...) have only the no-pivot entry,
// which is their overall total; use tallyRoll() for a single number.
//
BetOutcomeStats
BetStats::tally(BetName name, BetScope scope) const
{
    BetOutcomeStats result;
    const std::string base = statsName(name);

    if (name == BetName::Field)
    {
        // Field is recorded per number rolled (Field2 .. Field12). Scope
        // has no meaning for it, always return the total.
        if (const auto it = byBet.find(base); it != byBet.end())
            result.merge(it->second);
        for (unsigned roll = 2; roll <= 12; ++roll)
            if (const auto it = byBet.find(base + std::to_string(roll));
                it != byBet.end())
                result.merge(it->second);
        return result;
    }

    if (isRollBet(name))
    {
        // Other one roll bets only have the overall entry.
        if (const auto it = byBet.find(base); it != byBet.end())
            result.merge(it->second);
        return result;
    }

    if (scope != BetScope::Points)
        if (const auto it = byBet.find(base); it != byBet.end())
            result.merge(it->second);

    if (scope != BetScope::ComeOut)
        for (const unsigned point : PointNumbers)
            if (const auto it = byBet.find(base + std::to_string(point));
                it != byBet.end())
                result.merge(it->second);
    return result;
}

//-----------------------------------------------------------------
//
// Outcomes for a bet with the given point/pivot, e.g. PassLine6.
//
BetOutcomeStats
BetStats::tallyPoint(BetName name, unsigned point) const
{
    if (const auto it = byBet.find(statsName(name) + std::to_string(point));
        it != byBet.end())
        return it->second;
    return {};
}

//-----------------------------------------------------------------
//
// Outcomes of a roll dependent bet when a particular number was rolled,
// e.g. Field wins on 2 or Field losses on 7.
//
BetOutcomeStats
BetStats::tallyRoll(BetName name, unsigned roll) const
{
    if (!isRollBet(name))
        return {};
    if (const auto it = byBet.find(rollEntryName(name, roll));
        it != byBet.end())
        return it->second;
    return {};
}

//-----------------------------------------------------------------

BetOutcomeStats
BetStats::tallyDoSide(BetScope scope) const
{
    BetOutcomeStats result = tally(BetName::PassLine, scope);
    result.merge(tally(BetName::Come, scope));
    return result;
}

//-----------------------------------------------------------------

BetOutcomeStats
BetStats::tallyDontSide(BetScope scope) const
{
    BetOutcomeStats result = tally(BetName::DontPass, scope);
    result.merge(tally(BetName::DontCome, scope));
    return result;
}

//-----------------------------------------------------------------

void
BetStats::reset()
{
    totNumBetsAllBets = 0;
    totNumWinsAllBets = 0;
    totNumLoseAllBets = 0;
    totNumKeepAllBets = 0;
    totAmtAllBets     = 0;
    totAmtWinsAllBets = 0;
    totAmtLoseAllBets = 0;
    totAmtKeepAllBets = 0;
    maxAmtBetOneBet   = 0;
    maxAmtWinOneBet   = 0;
    maxAmtLoseOneBet  = 0;
    maxAmtKeepOneBet  = 0;
    for (auto& [name, stats] : byBet)
    {
        (void) name;
        stats.reset();
    }
    numBetsOneRoll.reset();
    numBetsWinOneRoll.reset();
    numBetsLoseOneRoll.reset();
    numBetsKeepOneRoll.reset();
    amtBetsOneRoll.reset();
    amtBetsWinOneRoll.reset();
    amtBetsLoseOneRoll.reset();
    amtBetsKeepOneRoll.reset();
}

//-----------------------------------------------------------------

void
BetStats::merge(const BetStats& session)
{
    totNumBetsAllBets += session.totNumBetsAllBets;
    totNumWinsAllBets += session.totNumWinsAllBets;
    totNumLoseAllBets += session.totNumLoseAllBets;
    totNumKeepAllBets += session.totNumKeepAllBets;
    totAmtAllBets     += session.totAmtAllBets;
    totAmtWinsAllBets += session.totAmtWinsAllBets;
    totAmtLoseAllBets += session.totAmtLoseAllBets;
    totAmtKeepAllBets += session.totAmtKeepAllBets;
    maxAmtBetOneBet   = std::max(maxAmtBetOneBet,  session.maxAmtBetOneBet);
    maxAmtWinOneBet   = std::max(maxAmtWinOneBet,  session.maxAmtWinOneBet);
    maxAmtLoseOneBet  = std::max(maxAmtLoseOneBet, session.maxAmtLoseOneBet);
    maxAmtKeepOneBet  = std::max(maxAmtKeepOneBet, session.maxAmtKeepOneBet);

    for (const auto& [name, stats] : session.byBet)
        byBet[name].merge(stats);

    numBetsOneRoll.merge    (session.numBetsOneRoll);
    numBetsWinOneRoll.merge (session.numBetsWinOneRoll);
    numBetsLoseOneRoll.merge(session.numBetsLoseOneRoll);
    numBetsKeepOneRoll.merge(session.numBetsKeepOneRoll);
    amtBetsOneRoll.merge    (session.amtBetsOneRoll);
    amtBetsWinOneRoll.merge (session.amtBetsWinOneRoll);
    amtBetsLoseOneRoll.merge(session.amtBetsLoseOneRoll);
    amtBetsKeepOneRoll.merge(session.amtBetsKeepOneRoll);
}

//-----------------------------------------------------------------

YAML::Node
BetStats::toYAML() const
{
    YAML::Node node;
    YAML::Node overall;
    YAML::Node bets;
    bets["count"] = totNumBetsAllBets;
    bets["wins"] = totNumWinsAllBets;
    bets["losses"] = totNumLoseAllBets;
    bets["kept"] = totNumKeepAllBets;
    overall["bets"] = bets;

    YAML::Node amounts;
    amounts["bet"] = totAmtAllBets;
    amounts["won"] = totAmtWinsAllBets;
    amounts["lost"] = totAmtLoseAllBets;
    amounts["kept"] = totAmtKeepAllBets;
    overall["amounts"] = amounts;

    YAML::Node maximums;
    maximums["bet"] = maxAmtBetOneBet;
    maximums["win"] = maxAmtWinOneBet;
    maximums["loss"] = maxAmtLoseOneBet;
    maximums["kept"] = maxAmtKeepOneBet;
    overall["maximums"] = maximums;
    node["overall"] = overall;

    YAML::Node byBetNode;
    for (const auto& [name, stats] : byBet)
        byBetNode[name] = stats.toYAML();
    node["byBet"] = byBetNode;

    YAML::Node perRoll;
    perRoll["bets"] = numBetsOneRoll.toYAML();
    perRoll["wins"] = numBetsWinOneRoll.toYAML();
    perRoll["losses"] = numBetsLoseOneRoll.toYAML();
    perRoll["kept"] = numBetsKeepOneRoll.toYAML();

    YAML::Node rollAmounts;
    rollAmounts["bet"] = amtBetsOneRoll.toYAML();
    rollAmounts["won"] = amtBetsWinOneRoll.toYAML();
    rollAmounts["lost"] = amtBetsLoseOneRoll.toYAML();
    rollAmounts["kept"] = amtBetsKeepOneRoll.toYAML();
    perRoll["amounts"] = rollAmounts;
    node["perRoll"] = perRoll;
    return node;
}

//-----------------------------------------------------------------

void
BetStats::fromYAML(const YAML::Node& node)
{
    const YAML::Node overall = node["overall"];
    const YAML::Node bets = overall["bets"];
    totNumBetsAllBets = bets["count"].as<unsigned>();
    totNumWinsAllBets = bets["wins"].as<unsigned>();
    totNumLoseAllBets = bets["losses"].as<unsigned>();
    totNumKeepAllBets = bets["kept"].as<unsigned>();

    const YAML::Node amounts = overall["amounts"];
    totAmtAllBets = amounts["bet"].as<Gen::Money>();
    totAmtWinsAllBets = amounts["won"].as<Gen::Money>();
    totAmtLoseAllBets = amounts["lost"].as<Gen::Money>();
    totAmtKeepAllBets = amounts["kept"].as<Gen::Money>();

    const YAML::Node maximums = overall["maximums"];
    maxAmtBetOneBet = maximums["bet"].as<Gen::Money>();
    maxAmtWinOneBet = maximums["win"].as<Gen::Money>();
    maxAmtLoseOneBet = maximums["loss"].as<Gen::Money>();
    maxAmtKeepOneBet = maximums["kept"].as<Gen::Money>();

    initializeBetMap(byBet);
    const YAML::Node byBetNode = node["byBet"];
    for (auto it = byBetNode.begin(); it != byBetNode.end(); ++it)
    {
        const std::string name = it->first.as<std::string>();
        byBet[name].fromYAML(it->second);
    }

    const YAML::Node perRoll = node["perRoll"];
    numBetsOneRoll.fromYAML(perRoll["bets"]);
    numBetsWinOneRoll.fromYAML(perRoll["wins"]);
    numBetsLoseOneRoll.fromYAML(perRoll["losses"]);
    numBetsKeepOneRoll.fromYAML(perRoll["kept"]);

    const YAML::Node rollAmounts = perRoll["amounts"];
    amtBetsOneRoll.fromYAML(rollAmounts["bet"]);
    amtBetsWinOneRoll.fromYAML(rollAmounts["won"]);
    amtBetsLoseOneRoll.fromYAML(rollAmounts["lost"]);
    amtBetsKeepOneRoll.fromYAML(rollAmounts["kept"]);
}

//-----------------------------------------------------------------
