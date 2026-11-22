//----------------------------------------------------------------
//
// File: BetSpec.cpp
//
//----------------------------------------------------------------

#include <controller/BetSpec.h>

using namespace Ctrl;

//----------------------------------------------------------------

std::string
Ctrl::describe(const BetSpec& spec)
{
    std::string s = EnumBetName::toString(spec.betName);
    if (spec.pivot != 0)
    {
        s += " " + std::to_string(spec.pivot);
    }
    if (spec.isOdds)
    {
        s += " odds";
    }
    s += " " + Gen::MoneyUtils::toString(spec.amount);
    return s;
}

//----------------------------------------------------------------

std::string
Ctrl::describe(const BetPlan& plan)
{
    std::string s;
    for (const auto& spec : plan.specs)
    {
        if (!s.empty()) s += ", ";
        s += describe(spec);
    }
    return s;
}

//----------------------------------------------------------------

bool
Ctrl::isPlanOddsBet(BetName betName)
{
    return betName == BetName::PassLine || betName == BetName::DontPass;
}

//----------------------------------------------------------------
