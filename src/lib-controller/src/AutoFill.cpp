//----------------------------------------------------------------
//
// File: AutoFill.cpp
//
//----------------------------------------------------------------

#include <controller/AutoFill.h>

using namespace Ctrl;

//----------------------------------------------------------------

AutoFill* AutoFill::instance()
{
    static AutoFill autoFill;
    return &autoFill;
}

//----------------------------------------------------------------

Gen::ReturnCode
AutoFill::getAutoFill(AutoFillEntry& afe)
{
    // Look for an existing entry with same betName, pivot, and isOddsBet
    auto it = std::find_if(fills_.begin(), fills_.end(),
        [&](const AutoFillEntry& existing) {
            return existing.betName   == afe.betName
                && existing.pivot     == afe.pivot
                && existing.isOddsBet == afe.isOddsBet;
        });

    if (it != fills_.end())
    {
        afe.amount = it->amount;  // Replace amount
        return Gen::ReturnCode::Success;
    }
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

void
AutoFill::setAutoFill(const AutoFillEntry& afe)
{
    // Look for an existing entry with same betName, pivot, and isOddsBet
    auto it = std::find_if(fills_.begin(), fills_.end(),
        [&](const AutoFillEntry& existing) {
            return existing.betName   == afe.betName
                && existing.pivot     == afe.pivot
                && existing.isOddsBet == afe.isOddsBet;
        });

    if (it != fills_.end())
    {
        it->amount = afe.amount;  // Replace amount only
    }
    else
    {
        fills_.push_back(afe);    // No match: add new entry
    }
}

//----------------------------------------------------------------

void
AutoFill::deleteAutoFill(const AutoFillEntry& afe)
{
    auto it = std::find_if(fills_.begin(), fills_.end(),
        [&](const AutoFillEntry& existing) {
            return existing.betName   == afe.betName
                && existing.pivot     == afe.pivot
                && existing.isOddsBet == afe.isOddsBet;
        });

    if (it != fills_.end())
    {
        // Erase compacts the vector automatically (no "holes")
        fills_.erase(it);
    }
}

//----------------------------------------------------------------

const AutoFill::Fills&
AutoFill::getAutoFills()
{
    return fills_;
}

//----------------------------------------------------------------
