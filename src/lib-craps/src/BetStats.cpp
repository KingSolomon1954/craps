//----------------------------------------------------------------
//
// File: BetStats.cpp
//
//----------------------------------------------------------------

#include <craps/BetStats.h>
#include <craps/CrapsBetIntfc.h>

using namespace Craps;

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
    // Populate win/lose maps will all bets. Don't want any empty's.
    // Stats are therefore accessible and zero, not empty.
    // Avoid many "if" checks later.

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

    YAML::Node winNode;
    for (const auto& [key, stat] : wins)
    {
        winNode[key] = stat.toYAML();
    }
    node["wins"] = winNode;

    YAML::Node loseNode;
    for (const auto& [key, stat] : lose)
    {
        loseNode[key] = stat.toYAML();
    }
    node["lose"] = loseNode;

    return node;
}

//-----------------------------------------------------------------

void
BetTypeStats::fromYAML(const YAML::Node& node)
{
    // Clear any existing data
    wins.clear();
    lose.clear();

    const YAML::Node& winsNode = node["wins"];
    for (auto it = winsNode.begin(); it != winsNode.end(); ++it)
    {
        std::string key = it->first.as<std::string>();
        BetStat stat;
        stat.fromYAML(it->second);
        wins[key] = stat;
    }

    const YAML::Node& loseNode = node["lose"];
    for (auto it = loseNode.begin(); it != loseNode.end(); ++it)
    {
        std::string key = it->first.as<std::string>();
        BetStat stat;
        stat.fromYAML(it->second);
        lose[key] = stat;
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
    max   = node["max"].as<unsigned>();
    total = node["total"].as<unsigned>();
    current = 0;
}

//----------------------------------------------------------------
//
// Helper function to expand bet names with their pivot.
//
// PassLine --> PassLine6
// Come     --> Come8
// Hardway  --> Hardway4
//
std::string
BetStats::expandBetName(const CrapsBetIntfc& bet) const
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
    maxAmtBetOneBet   = std::max(maxAmtBetOneBet,  session.maxAmtBetOneBet);
    maxAmtWinOneBet   = std::max(maxAmtWinOneBet,  session.maxAmtWinOneBet);
    maxAmtLoseOneBet  = std::max(maxAmtLoseOneBet, session.maxAmtLoseOneBet);
    maxAmtKeepOneBet  = std::max(maxAmtLoseOneBet, session.maxAmtKeepOneBet);

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

    node["totNumBetsAllBets"]  = totNumBetsAllBets;
    node["totNumWinsAllBets"]  = totNumWinsAllBets;
    node["totNumLoseAllBets"]  = totNumLoseAllBets;
    node["totNumKeepAllBets"]  = totNumKeepAllBets;
    node["totAmtAllBets"]      = totAmtAllBets;
    node["totAmtWinsAllBets"]  = totAmtWinsAllBets;
    node["totAmtLoseAllBets"]  = totAmtLoseAllBets;
    node["totAmtKeepAllBets"]  = totAmtKeepAllBets;
    node["maxAmtBetOneBet"]    = maxAmtBetOneBet;
    node["maxAmtBetOneBet"]    = maxAmtWinOneBet;
    node["maxAmtWinOneBet"]    = maxAmtWinOneBet;
    node["maxAmtLoseOneBet"]   = maxAmtLoseOneBet;
    node["maxAmtKeepOneBet"]   = maxAmtKeepOneBet;

    node["betTypeStats"]       = betTypeStats.toYAML();

    node["numBetsOneRoll"]     = numBetsOneRoll.toYAML();
    node["numBetsWinOneRoll"]  = numBetsWinOneRoll.toYAML();
    node["numBetsLoseOneRoll"] = numBetsLoseOneRoll.toYAML();
    node["numBetsKeepOneRoll"] = numBetsKeepOneRoll.toYAML();
    node["amtBetsOneRoll"]     = amtBetsOneRoll.toYAML();
    node["amtBetsWinOneRoll"]  = amtBetsWinOneRoll.toYAML();
    node["amtBetsLoseOneRoll"] = amtBetsLoseOneRoll.toYAML();
    node["amtBetsKeepOneRoll"] = amtBetsKeepOneRoll.toYAML();
    return node;
}

//-----------------------------------------------------------------

void
BetStats::fromYAML(const YAML::Node& node)
{
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

    betTypeStats.fromYAML(node["betTypeStats"]);

    numBetsOneRoll.fromYAML    (node["numBetsOneRoll"]);
    numBetsWinOneRoll.fromYAML (node["numBetsWinOneRoll"]);
    numBetsLoseOneRoll.fromYAML(node["numBetsLoseOneRoll"]);
    numBetsKeepOneRoll.fromYAML(node["numBetsKeepOneRoll"]);
    amtBetsOneRoll.fromYAML    (node["amtBetsOneRoll"]);
    amtBetsWinOneRoll.fromYAML (node["amtBetsWinOneRoll"]);
    amtBetsLoseOneRoll.fromYAML(node["amtBetsLoseOneRoll"]);
    amtBetsKeepOneRoll.fromYAML(node["amtBetsKeepOneRoll"]);
}

//-----------------------------------------------------------------
