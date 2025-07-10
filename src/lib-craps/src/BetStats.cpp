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
    if (node["count"])       count       = node["count"].as<unsigned>();
    if (node["totDistance"]) totDistance = node["totDistance"].as<unsigned>();
    if (node["amount"])      amount      = node["amount"].as<unsigned>();
    if (node["amountBet"])   amountBet   = node["amountBet"].as<unsigned>();
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

    if (node["wins"])
    {
        const YAML::Node& winsNode = node["wins"];
        for (auto it = winsNode.begin(); it != winsNode.end(); ++it)
        {
            std::string key = it->first.as<std::string>();
            BetStat stat;
            stat.fromYAML(it->second);
            wins[key] = stat;
        }
    }

    if (node["lose"])
    {
        const YAML::Node& loseNode = node["lose"];
        for (auto it = loseNode.begin(); it != loseNode.end(); ++it)
        {
            std::string key = it->first.as<std::string>();
            BetStat stat;
            stat.fromYAML(it->second);
            lose[key] = stat;
        }
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
    if (node["max"])   max   = node["max"].as<unsigned>();
    if (node["total"]) total = node["total"].as<unsigned>();
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
    if (node["max"])   max   = node["max"].as<unsigned>();
    if (node["total"]) total = node["total"].as<unsigned>();
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
    betTypeStats.reset();
    
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
    betTypeStats.merge(session.betTypeStats);
    
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
    node["betTypeStats"]       = betTypeStats.toYAML();
    
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
    node["maxAmtLoseOneBet"]   = maxAmtLoseOneBet;
    node["maxAmtKeepOneBet"]   = maxAmtKeepOneBet;
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
    if (node["betTypeStats"]) betTypeStats.fromYAML(node["betTypeStats"]);

    if (node["totNumBetsAllBets"])  totNumBetsAllBets = node["totNumBetsAllBets"].as<unsigned>();
    if (node["totNumWinsAllBets"])  totNumWinsAllBets = node["totNumWinsAllBets"].as<unsigned>();
    if (node["totNumLoseAllBets"])  totNumLoseAllBets = node["totNumLoseAllBets"].as<unsigned>();
    if (node["totNumKeepAllBets"])  totNumKeepAllBets = node["totNumKeepAllBets"].as<unsigned>();
    if (node["totAmtAllBets"])      totAmtAllBets     = node["totAmtAllBets"].as<unsigned>();
    if (node["totAmtWinsAllBets"])  totAmtWinsAllBets = node["totAmtWinsAllBets"].as<unsigned>();
    if (node["totAmtLoseAllBets"])  totAmtLoseAllBets = node["totAmtLoseAllBets"].as<unsigned>();
    if (node["totAmtKeepAllBets"])  totAmtKeepAllBets = node["totAmtKeepAllBets"].as<unsigned>();
    if (node["maxAmtBetOneBet"])    maxAmtBetOneBet   = node["maxAmtBetOneBet"].as<unsigned>();
    if (node["maxAmtWinOneBet"])    maxAmtWinOneBet   = node["maxAmtWinOneBet"].as<unsigned>();
    if (node["maxAmtLoseOneBet"])   maxAmtLoseOneBet  = node["maxAmtLoseOneBet"].as<unsigned>();
    if (node["maxAmtKeepOneBet"])   maxAmtKeepOneBet  = node["maxAmtKeepOneBet"].as<unsigned>();
    
    if (node["numBetsOneRoll"])     numBetsOneRoll.fromYAML    (node["numBetsOneRoll"]);
    if (node["numBetsWinOneRoll"])  numBetsWinOneRoll.fromYAML (node["numBetsWinOneRoll"]);
    if (node["numBetsLoseOneRoll"]) numBetsLoseOneRoll.fromYAML(node["numBetsLoseOneRoll"]);
    if (node["numBetsKeepOneRoll"]) numBetsKeepOneRoll.fromYAML(node["numBetsKeepOneRoll"]);
    if (node["amtBetsOneRoll"])     amtBetsOneRoll.fromYAML    (node["amtBetsOneRoll"]);
    if (node["amtBetsWinOneRoll"])  amtBetsWinOneRoll.fromYAML (node["amtBetsWinOneRoll"]);
    if (node["amtBetsLoseOneRoll"]) amtBetsLoseOneRoll.fromYAML(node["amtBetsLoseOneRoll"]);
    if (node["amtBetsKeepOneRoll"]) amtBetsKeepOneRoll.fromYAML(node["amtBetsKeepOneRoll"]);
}

//-----------------------------------------------------------------
