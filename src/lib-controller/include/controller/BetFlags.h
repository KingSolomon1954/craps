//----------------------------------------------------------------
//
// File: BetFlags.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/CrapsBet.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
#include <string>
#include <utility>

namespace Ctrl {

namespace BetFlags {

/// What a request applies to
enum class Scope { Bet, All, Odds, Place, Buy, Lay, Hardway };

/// What a request does
enum class Change
{
    NotWorking,
    NotWorkingNextRoll,
    Working,
    WorkingNextRoll,
    TableDefault
};

/// The category a flaggable bet is in. Not for Field and the like.
enum class Category { None, Odds, Place, Buy, Lay, Hardway };

/// The category of a bet by its name. PassLine, DontPass, Come and DontCome
/// are Odds, since their flags are for their odds.
Category categoryOf(BetName betName);

/// Can the bet be flagged right now, and does the scope include it? Scope
/// Bet is not decided here, the caller has the bet id.
bool inScope(Scope scope, const Craps::CrapsBet& bet);

/// The same for a category, for the menus
bool inScope(Scope scope, Category category);

/// The standing state and override the bet gets from the change.
std::pair<Craps::FlagStanding, Craps::FlagOneRoll>
newState(Change change, const Craps::CrapsBet& bet);

/// "Not-Working", "Not-Working Next Roll", "Working", "Working Next Roll",
/// "Table Default"
std::string changeText(Change change);

/// "all bets", "odds bets", "place bets", ...
std::string scopeText(Scope scope);

/// e.g. "Pass Line 6 odds $30", "Place 9 $20", "Hardway 4 $5"
std::string betLabel(const Craps::CrapsBet& bet);

/// e.g. "Default", "Not-Working", "Working, Not-Working Next Roll"
std::string flagText(const Craps::CrapsBet& bet);

} // namespace BetFlags

/*-----------------------------------------------------------*//**

@namespace Ctrl::BetFlags

@brief Rules and words for bet flags that need no table

Which bets a scope includes, what each change does to a bet's flags, and
the text the menus show. See "Bet Flags" in the design document.

*/

} // namespace Ctrl

//----------------------------------------------------------------
