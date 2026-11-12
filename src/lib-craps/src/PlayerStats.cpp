//----------------------------------------------------------------
//
// File: PlayerStats.cpp
//
//----------------------------------------------------------------

#include <craps/PlayerStats.h>
// #include <craps/EnumBetName.h>

using namespace Craps;

/*-----------------------------------------------------------*//**

Record a winning bet.

*/
void
PlayerStats::recordWin(const CrapsBet& bet, Gen::Money amtWin, unsigned roll)
{
    betStats.recordWin(bet, amtWin, roll);
}

/*-----------------------------------------------------------*//**

Record a losing bet.

*/
void
PlayerStats::recordLose(const CrapsBet& bet, Gen::Money amtLose, unsigned roll)
{
    betStats.recordLose(bet, amtLose, roll);
}

/*-----------------------------------------------------------*//**

Record a winning bet from its decision record. Preferred over the
overload above since it also records roll, odds and component details.

*/
void
PlayerStats::recordWin(const DecisionRecord& dr)
{
    betStats.recordWin(dr);
}

/*-----------------------------------------------------------*//**

Record a losing bet from its decision record.

*/
void
PlayerStats::recordLose(const DecisionRecord& dr)
{
    betStats.recordLose(dr);
}

/*-----------------------------------------------------------*//**

Record a keeping bet.

No need to count keeps. They will eventually win or lose.
But might want to track avg number of keeps per roll.

*/
void
PlayerStats::recordKeep(const CrapsBet& bet)
{
    betStats.recordKeep(bet);
}

//-----------------------------------------------------------------

void
PlayerStats::reset()
{
    betStats.reset();
    // No sessionHistory.clear()
}

//-----------------------------------------------------------------

void
PlayerStats::merge(const PlayerStats& session)
{
    betStats.merge(session.betStats);
    // No sessionHistory.merge()
}

//-----------------------------------------------------------------

void
PlayerStats::toYAML(YAML::Node& node) const
{
    node["BetStats"]     = betStats.toYAML();
    node["SessionStats"] = sessionHistory.toYAML();
}

//-----------------------------------------------------------------

void
PlayerStats::fromYAML(const YAML::Node& node)
{
    betStats.fromYAML      (node["BetStats"]);
    sessionHistory.fromYAML(node["SessionStats"]);
}

//-----------------------------------------------------------------
