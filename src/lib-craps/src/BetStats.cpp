//----------------------------------------------------------------
//
// File: BetStats.cpp
//
//----------------------------------------------------------------

#include <craps/BetStats.h>

using namespace Craps;

/*-----------------------------------------------------------*//**

Record a winning bet.

*/
void
BetStats::recordWin(const CrapsBet& bet, Gen::Money amtWin)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    std::string betName = expandBetName(bet);
    betTypeStats.wins[betName].count++;
    betTypeStats.wins[betName].amountBet   += amtBet;
    betTypeStats.wins[betName].amount      += amtWin;
    betTypeStats.wins[betName].totDistance += bet.distance();

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
BetStats::recordLose(const CrapsBet& bet, Gen::Money amtLose)
{
    unsigned amtBet = bet.contractAmount() + bet.oddsAmount();
    recordCommon(amtBet);

    std::string betName = expandBetName(bet);
    betTypeStats.lose[betName].count++;
    betTypeStats.lose[betName].amountBet   += amtBet;
    betTypeStats.lose[betName].amount      += amtLose;
    betTypeStats.lose[betName].totDistance += bet.distance();

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

BetTypeStats::BetTypeStats()
{
    // Populate win/lose maps with all bets. Don't want any empty slots.
    // All these stats are therefore accessible any time, not empty.
    // Avoids many "if" checks later.

    wins["PassLine"];      lose["PassLine"];
    wins["PassLine4"];     lose["PassLine4"];
    wins["PassLine5"];     lose["PassLine5"];
    wins["PassLine6"];     lose["PassLine6"];
    wins["PassLine8"];     lose["PassLine8"];
    wins["PassLine9"];     lose["PassLine9"];
    wins["PassLine10"];    lose["PassLine10"];
    wins["DontPass"];      lose["DontPass"];
    wins["DontPass4"];     lose["DontPass4"];
    wins["DontPass5"];     lose["DontPass5"];
    wins["DontPass6"];     lose["DontPass6"];
    wins["DontPass8"];     lose["DontPass8"];
    wins["DontPass9"];     lose["DontPass9"];
    wins["DontPass10"];    lose["DontPass10"];
    wins["Come"];          lose["Come"];
    wins["Come4"];         lose["Come4"];
    wins["Come5"];         lose["Come5"];
    wins["Come6"];         lose["Come6"];
    wins["Come8"];         lose["Come8"];
    wins["Come9"];         lose["Come9"];
    wins["Come10"];        lose["Come10"];
    wins["DontCome"];      lose["DontCome"];
    wins["DontCome4"];     lose["DontCome4"];
    wins["DontCome4"];     lose["DontCome4"];
    wins["DontCome5"];     lose["DontCome5"];
    wins["DontCome6"];     lose["DontCome6"];
    wins["DontCome8"];     lose["DontCome8"];
    wins["DontCome9"];     lose["DontCome9"];
    wins["DontCome10"];    lose["DontCome10"];
    wins["Place4"];        lose["Place4"];
    wins["Place5"];        lose["Place5"];
    wins["Place6"];        lose["Place6"];
    wins["Place8"];        lose["Place8"];
    wins["Place9"];        lose["Place9"];
    wins["Place10"];       lose["Place10"];
    wins["Lay4"];          lose["Lay4"];
    wins["Lay5"];          lose["Lay5"];
    wins["Lay6"];          lose["Lay6"];
    wins["Lay8"];          lose["Lay8"];
    wins["Lay9"];          lose["Lay9"];
    wins["Lay10"];         lose["Lay10"];
    wins["Buy4"];          lose["Buy4"];
    wins["Buy5"];          lose["Buy5"];
    wins["Buy6"];          lose["Buy6"];
    wins["Buy8"];          lose["Buy8"];
    wins["Buy9"];          lose["Buy9"];
    wins["Buy10"];         lose["Buy10"];
    wins["Hard4"];         lose["Hard4"];
    wins["Hard6"];         lose["Hard6"];
    wins["Hard8"];         lose["Hard8"];
    wins["Hard10"];        lose["Hard10"];
    wins["Field"];         lose["Field"];
    wins["AnyCraps"];      lose["AnyCraps"];
    wins["AnySeven"];      lose["AnySeven"];
    wins["CandE"];         lose["CandE"];
    wins["Horn"];          lose["Horn"];
    wins["World"];         lose["World"];
}

//-----------------------------------------------------------------

void
BetTypeStats::reset()
{
    for (auto& t : wins)
    {
        t.second.reset();
    }

    for (auto& t : lose)
    {
        t.second.reset();
    }
}

//-----------------------------------------------------------------

void
BetTypeStats::merge(const BetTypeStats& bts)
{
    for (const auto& [key, stat] : bts.wins)
    {
        wins[key].merge(stat);  // Will default construct if key not found
    }

    for (const auto& [key, stat] : bts.lose)
    {
        lose[key].merge(stat);  // Will default construct if key not found
    }
}

//-----------------------------------------------------------------

YAML::Node
BetTypeStats::toYAML() const
{
    YAML::Node node;

    // Keep each named bet in one place, with both outcomes underneath it.
    for (const auto& [key, winStat] : wins)
    {
        YAML::Node betNode;
        betNode["wins"] = winStat.toYAML();

        const auto loss = lose.find(key);
        betNode["losses"] =
            loss != lose.end() ? loss->second.toYAML() : BetStat{}.toYAML();

        node[key] = betNode;
    }

    // Preserve any loss-only keys, even if the maps become inconsistent.
    for (const auto& [key, lossStat] : lose)
    {
        if (wins.find(key) == wins.end())
        {
            YAML::Node betNode;
            betNode["wins"] = BetStat{}.toYAML();
            betNode["losses"] = lossStat.toYAML();
            node[key] = betNode;
        }
    }

    return node;
}

//-----------------------------------------------------------------

void
BetTypeStats::fromYAML(const YAML::Node& node)
{
    wins.clear();
    lose.clear();

    // New format:
    // byBet:
    //   PassLine:
    //     wins: ...
    //     losses: ...
    //
    // Legacy format:
    // wins: { PassLine: ... }
    // lose: { PassLine: ... }
    
    if (node["wins"] || node["lose"])
    {
        const YAML::Node& winsNode = node["wins"];
        if (winsNode)
        {
            for (auto it = winsNode.begin(); it != winsNode.end(); ++it)
            {
                const std::string key = it->first.as<std::string>();
                wins[key].fromYAML(it->second);
            }
        }

        const YAML::Node& loseNode = node["lose"];
        if (loseNode)
        {
            for (auto it = loseNode.begin(); it != loseNode.end(); ++it)
            {
                const std::string key = it->first.as<std::string>();
                lose[key].fromYAML(it->second);
            }
        }
    }
    else
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            const std::string key = it->first.as<std::string>();
            const YAML::Node& betNode = it->second;

            if (betNode["wins"])
                wins[key].fromYAML(betNode["wins"]);
            else
                wins[key] = BetStat{};

            if (betNode["losses"])
                lose[key].fromYAML(betNode["losses"]);
            else if (betNode["lose"]) // Accept an intermediate spelling too.
                lose[key].fromYAML(betNode["lose"]);
            else
                lose[key] = BetStat{};
        }
    }

    // Add newly supported bet names that aren't present in older saved data.
    const BetTypeStats defaults;
    for (const auto& [key, stat] : defaults.wins)
    {
        (void) stat;
        wins.try_emplace(key);
        lose.try_emplace(key);
    }
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
    max   += session.max;
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
    max   += session.max;
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
    max   = node["max"].as<Gen::Money>();
    total = node["total"].as<Gen::Money>();
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
    std::string betName = EnumBetName::toString(bet.betName());
    if (bet.pivot() != 0)
    {
        betName += std::to_string(bet.pivot());
    }
    return betName;
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
    betTypeStats.reset();
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
    maxAmtBetOneBet    = std::max(maxAmtBetOneBet,  session.maxAmtBetOneBet);
    maxAmtWinOneBet    = std::max(maxAmtWinOneBet,  session.maxAmtWinOneBet);
    maxAmtLoseOneBet   = std::max(maxAmtLoseOneBet, session.maxAmtLoseOneBet);
    maxAmtKeepOneBet   = std::max(maxAmtKeepOneBet, session.maxAmtKeepOneBet);

    betTypeStats.merge(session.betTypeStats);

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
    bets["count"]  = totNumBetsAllBets;
    bets["wins"]   = totNumWinsAllBets;
    bets["losses"] = totNumLoseAllBets;
    bets["kept"]   = totNumKeepAllBets;
    overall["bets"] = bets;

    YAML::Node amounts;
    amounts["bet"]    = totAmtAllBets;
    amounts["won"]    = totAmtWinsAllBets;
    amounts["lost"]   = totAmtLoseAllBets;
    amounts["kept"]   = totAmtKeepAllBets;
    overall["amounts"] = amounts;

    YAML::Node maximums;
    maximums["bet"]  = maxAmtBetOneBet;
    maximums["win"]  = maxAmtWinOneBet;
    maximums["loss"] = maxAmtLoseOneBet;
    maximums["kept"] = maxAmtKeepOneBet;
    overall["maximums"] = maximums;

    node["overall"] = overall;
    node["byBet"] = betTypeStats.toYAML();

    YAML::Node perRoll;
    YAML::Node rollBets;
    rollBets["total"] = numBetsOneRoll.total;
    rollBets["max"] = numBetsOneRoll.max;

    YAML::Node rollWins;
    rollWins["total"] = numBetsWinOneRoll.total;
    rollWins["max"] = numBetsWinOneRoll.max;
    rollBets["wins"] = rollWins;

    YAML::Node rollLosses;
    rollLosses["total"] = numBetsLoseOneRoll.total;
    rollLosses["max"] = numBetsLoseOneRoll.max;
    rollBets["losses"] = rollLosses;

    YAML::Node rollKept;
    rollKept["total"] = numBetsKeepOneRoll.total;
    rollKept["max"] = numBetsKeepOneRoll.max;
    rollBets["kept"] = rollKept;
    perRoll["bets"] = rollBets;

    YAML::Node rollAmounts;
    YAML::Node rollBetAmount;
    rollBetAmount["total"] = amtBetsOneRoll.total;
    rollBetAmount["max"] = amtBetsOneRoll.max;
    rollAmounts["bet"] = rollBetAmount;

    YAML::Node rollWonAmount;
    rollWonAmount["total"] = amtBetsWinOneRoll.total;
    rollWonAmount["max"] = amtBetsWinOneRoll.max;
    rollAmounts["won"] = rollWonAmount;

    YAML::Node rollLostAmount;
    rollLostAmount["total"] = amtBetsLoseOneRoll.total;
    rollLostAmount["max"] = amtBetsLoseOneRoll.max;
    rollAmounts["lost"] = rollLostAmount;

    YAML::Node rollKeptAmount;
    rollKeptAmount["total"] = amtBetsKeepOneRoll.total;
    rollKeptAmount["max"] = amtBetsKeepOneRoll.max;
    rollAmounts["kept"] = rollKeptAmount;

    perRoll["amounts"] = rollAmounts;
    node["perRoll"] = perRoll;

    return node;
}

//-----------------------------------------------------------------

void
BetStats::fromYAML(const YAML::Node& node)
{
    const YAML::Node overall = node["overall"];

    if (overall)
    {
        const YAML::Node bets = overall["bets"];
        totNumBetsAllBets = bets["count"].as<unsigned>();
        totNumWinsAllBets = bets["wins"].as<unsigned>();
        totNumLoseAllBets = bets["losses"].as<unsigned>();
        totNumKeepAllBets = bets["kept"].as<unsigned>();

        const YAML::Node amounts = overall["amounts"];
        totAmtAllBets     = amounts["bet"].as<Gen::Money>();
        totAmtWinsAllBets = amounts["won"].as<Gen::Money>();
        totAmtLoseAllBets = amounts["lost"].as<Gen::Money>();
        totAmtKeepAllBets = amounts["kept"].as<Gen::Money>();

        const YAML::Node maximums = overall["maximums"];
        maxAmtBetOneBet  = maximums["bet"].as<Gen::Money>();
        maxAmtWinOneBet  = maximums["win"].as<Gen::Money>();
        maxAmtLoseOneBet = maximums["loss"].as<Gen::Money>();
        maxAmtKeepOneBet = maximums["kept"].as<Gen::Money>();
    }
    else
    {
        // Legacy flat format.
        totNumBetsAllBets = node["totNumBetsAllBets"].as<unsigned>();
        totNumWinsAllBets = node["totNumWinsAllBets"].as<unsigned>();
        totNumLoseAllBets = node["totNumLoseAllBets"].as<unsigned>();
        totNumKeepAllBets = node["totNumKeepAllBets"].as<unsigned>();
        totAmtAllBets     = node["totAmtAllBets"].as<Gen::Money>();
        totAmtWinsAllBets = node["totAmtWinsAllBets"].as<Gen::Money>();
        totAmtLoseAllBets = node["totAmtLoseAllBets"].as<Gen::Money>();
        totAmtKeepAllBets = node["totAmtKeepAllBets"].as<Gen::Money>();
        maxAmtBetOneBet   = node["maxAmtBetOneBet"].as<Gen::Money>();
        maxAmtWinOneBet   = node["maxAmtWinOneBet"].as<Gen::Money>();
        maxAmtLoseOneBet  = node["maxAmtLoseOneBet"].as<Gen::Money>();
        maxAmtKeepOneBet  = node["maxAmtKeepOneBet"].as<Gen::Money>();
    }

    const YAML::Node byBet = node["byBet"];
    if (byBet)
    {
        betTypeStats.fromYAML(byBet);
    }
    else
    {
        betTypeStats.fromYAML(node["betTypeStats"]);
    }

    const YAML::Node perRoll = node["perRoll"];
    if (perRoll)
    {
        const YAML::Node bets = perRoll["bets"];
        numBetsOneRoll.total = bets["total"].as<unsigned>();
        numBetsOneRoll.max = bets["max"].as<unsigned>();

        const YAML::Node wins = bets["wins"];
        numBetsWinOneRoll.total = wins["total"].as<unsigned>();
        numBetsWinOneRoll.max = wins["max"].as<unsigned>();

        const YAML::Node losses = bets["losses"];
        numBetsLoseOneRoll.total = losses["total"].as<unsigned>();
        numBetsLoseOneRoll.max = losses["max"].as<unsigned>();

        const YAML::Node kept = bets["kept"];
        numBetsKeepOneRoll.total = kept["total"].as<unsigned>();
        numBetsKeepOneRoll.max = kept["max"].as<unsigned>();

        const YAML::Node amounts = perRoll["amounts"];
        const auto readAmount = [](const YAML::Node& amountNode, AmtBets& stats)
        {
            stats.total = amountNode["total"].as<Gen::Money>();
            stats.max = amountNode["max"].as<Gen::Money>();
            stats.current = 0;
        };

        readAmount(amounts["bet"], amtBetsOneRoll);
        readAmount(amounts["won"], amtBetsWinOneRoll);
        readAmount(amounts["lost"], amtBetsLoseOneRoll);
        readAmount(amounts["kept"], amtBetsKeepOneRoll);

        numBetsOneRoll.current = 0;
        numBetsWinOneRoll.current = 0;
        numBetsLoseOneRoll.current = 0;
        numBetsKeepOneRoll.current = 0;
    }
    else
    {
        // Legacy per-roll fields.
        numBetsOneRoll.fromYAML(node["numBetsOneRoll"]);
        numBetsWinOneRoll.fromYAML(node["numBetsWinOneRoll"]);
        numBetsLoseOneRoll.fromYAML(node["numBetsLoseOneRoll"]);
        numBetsKeepOneRoll.fromYAML(node["numBetsKeepOneRoll"]);
        amtBetsOneRoll.fromYAML(node["amtBetsOneRoll"]);
        amtBetsWinOneRoll.fromYAML(node["amtBetsWinOneRoll"]);
        amtBetsLoseOneRoll.fromYAML(node["amtBetsLoseOneRoll"]);
        amtBetsKeepOneRoll.fromYAML(node["amtBetsKeepOneRoll"]);
    }
}

//-----------------------------------------------------------------
