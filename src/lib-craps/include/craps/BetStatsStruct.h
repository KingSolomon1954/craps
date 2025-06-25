//----------------------------------------------------------------
//
// File: BetStatsStruct.h
//
//----------------------------------------------------------------

#pragma once

#include <map>
#include <controller/Globals.h>

namespace Craps {

struct BetStat
{
    unsigned count = 0;
    unsigned totDistance = 0;
    Gbl::Money amount;
    Gbl::Money amountBet;
};

struct BetTypeStats
{
    std::map<std::string, BetStat> wins;
    std::map<std::string, BetStat> lose;
};

struct NumBets
{
    unsigned current = 0;
    unsigned max     = 0;
    unsigned total   = 0;
    void reset();
};

struct AmtBets
{
    Gbl::Money current = 0;
    Gbl::Money max     = 0;
    Gbl::Money total   = 0;
    void reset();
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

    Gbl::Money totAmtAllBets     = 0;
    Gbl::Money totAmtWinsAllBets = 0;
    Gbl::Money totAmtLoseAllBets = 0;
    Gbl::Money totAmtKeepAllBets = 0;

    Gbl::Money maxAmtBetOneBet  = 0;
    Gbl::Money maxAmtWinOneBet  = 0;
    Gbl::Money maxAmtLoseOneBet = 0;
    Gbl::Money maxAmtKeepOneBet = 0;

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
};

} // namespace Craps

//----------------------------------------------------------------
