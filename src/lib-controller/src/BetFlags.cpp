//----------------------------------------------------------------
//
// File: BetFlags.cpp
//
//----------------------------------------------------------------

#include <controller/BetFlags.h>

using namespace Ctrl;
using namespace Ctrl::BetFlags;
using Craps::FlagOneRoll;
using Craps::FlagStanding;

//----------------------------------------------------------------

Category
BetFlags::categoryOf(BetName betName)
{
    switch (betName)
    {
    case BetName::PassLine:
    case BetName::DontPass:
    case BetName::Come:
    case BetName::DontCome: return Category::Odds;
    case BetName::Place:    return Category::Place;
    case BetName::Buy:      return Category::Buy;
    case BetName::Lay:      return Category::Lay;
    case BetName::Hardway:  return Category::Hardway;
    default:                return Category::None;
    }
}

//----------------------------------------------------------------

bool
BetFlags::inScope(Scope scope, Category category)
{
    if (category == Category::None)
    {
        return false;
    }
    switch (scope)
    {
    case Scope::Bet:     return false;      // The caller has the bet id
    case Scope::All:     return true;
    case Scope::Odds:    return category == Category::Odds;
    case Scope::Place:   return category == Category::Place;
    case Scope::Buy:     return category == Category::Buy;
    case Scope::Lay:     return category == Category::Lay;
    case Scope::Hardway: return category == Category::Hardway;
    }
    return false;
}

//----------------------------------------------------------------

bool
BetFlags::inScope(Scope scope, const Craps::CrapsBet& bet)
{
    return bet.canTakeFlag() && inScope(scope, categoryOf(bet.betName()));
}

//----------------------------------------------------------------

std::pair<FlagStanding, FlagOneRoll>
BetFlags::newState(Change change, const Craps::CrapsBet& bet)
{
    switch (change)
    {
    case Change::NotWorking:
        return {FlagStanding::Off, FlagOneRoll::None};
    case Change::NotWorkingNextRoll:
        return {bet.flagStanding(), FlagOneRoll::Off};
    case Change::Working:
        return {FlagStanding::Working, FlagOneRoll::None};
    case Change::WorkingNextRoll:
        return {bet.flagStanding(), FlagOneRoll::Working};
    case Change::TableDefault:
        return {FlagStanding::Default, FlagOneRoll::None};
    }
    return {bet.flagStanding(), bet.flagOneRoll()};
}

//----------------------------------------------------------------

std::string
BetFlags::changeText(Change change)
{
    switch (change)
    {
    case Change::NotWorking:         return "Not-Working";
    case Change::NotWorkingNextRoll: return "Not-Working Next Roll";
    case Change::Working:            return "Working";
    case Change::WorkingNextRoll:    return "Working Next Roll";
    case Change::TableDefault:       return "Table Default";
    }
    return "";
}

//----------------------------------------------------------------

std::string
BetFlags::scopeText(Scope scope)
{
    switch (scope)
    {
    case Scope::Bet:     return "bet";
    case Scope::All:     return "all bets";
    case Scope::Odds:    return "odds bets";
    case Scope::Place:   return "place bets";
    case Scope::Buy:     return "buy bets";
    case Scope::Lay:     return "lay bets";
    case Scope::Hardway: return "hardway bets";
    }
    return "";
}

//----------------------------------------------------------------

std::string
BetFlags::betLabel(const Craps::CrapsBet& bet)
{
    std::string s;
    switch (bet.betName())
    {
    case BetName::PassLine: s = "Pass Line"; break;
    case BetName::DontPass: s = "Don't Pass"; break;
    case BetName::DontCome: s = "Don't Come"; break;
    default:                s = EnumBetName::toString(bet.betName()); break;
    }
    if (bet.pivot() != 0)
    {
        s += " " + std::to_string(bet.pivot());
    }
    if (categoryOf(bet.betName()) == Category::Odds)
    {
        return s + " odds " + Gen::MoneyUtils::toString(bet.oddsAmount());
    }
    return s + " " + Gen::MoneyUtils::toString(bet.contractAmount());
}

//----------------------------------------------------------------

std::string
BetFlags::flagText(const Craps::CrapsBet& bet)
{
    std::string s;
    switch (bet.flagStanding())
    {
    case FlagStanding::Default: s = "Table Default"; break;
    case FlagStanding::Working: s = "Working"; break;
    case FlagStanding::Off:     s = "Not-Working"; break;
    }
    switch (bet.flagOneRoll())
    {
    case FlagOneRoll::None:    break;
    case FlagOneRoll::Off:     s += ", Not-Working Next Roll"; break;
    case FlagOneRoll::Working: s += ", Working Next Roll"; break;
    }
    return s;
}

//----------------------------------------------------------------
