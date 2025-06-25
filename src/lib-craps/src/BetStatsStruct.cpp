//----------------------------------------------------------------
//
// File: BetStatsStruct.cpp
//
//----------------------------------------------------------------

#include <craps/BetStatsStruct.h>
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
        lose[key].merge(stat);  // @ill default construct if key not found
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
